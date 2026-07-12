// test_intercept.cpp — Test delayed key remapping (A→B)
// Flow:
//   1. Register A→B remap
//   2. Start input tracking to detect key events
//   3. Wait for physical A press (intercepted, queued)
//   4. Press 'y' to trigger flush_remap_output()
//   5. Detect whether B was actually sent
//   6. Report result and exit

#include <windows.h>
#include <stdio.h>
#include <stdint.h>
#include <conio.h>
#include <atomic>
#include <thread>
#include <mutex>
#include <vector>
#include <chrono>

#include "hid_controller.h"

// ==================== Input tracking state ====================

static std::atomic<bool> g_key_a_down{false};
static std::atomic<bool> g_flush_requested{false};
static std::atomic<bool> g_b_detected{false};

static void on_keyboard_event(uintptr_t, uint16_t vkey, bool is_down) {
    // Track physical A (VK 0x41)
    if (vkey == 'A') {
        if (is_down && !g_key_a_down.load()) {
            g_key_a_down.store(true);
            printf("\n[EVENT] Key A pressed (intercepted) — queued as B\n");
        }
    }
    // Track B after flush (from Logitech driver injection)
    if (vkey == 'B' && is_down && g_flush_requested.load() && !g_b_detected.load()) {
        g_b_detected.store(true);
        printf("\n[EVENT] Key B detected! (remap succeeded)\n");
    }
}

// ==================== Main ====================

int main() {
    printf("=== HIDController — Delayed Key Remapping Test ===\n");
    printf("Remap: A → B\n\n");

    // Step 0: Start input tracking (hidden window, RAW INPUT API)
    printf("[1] Starting input tracking...\n");
    if (!start_input_tracking(nullptr, on_keyboard_event, nullptr, nullptr)) {
        printf("Failed to start input tracking.\n");
        return 1;
    }

    // Step 1: Register A→B remap
    set_intercept_device(0);
    printf("[2] Registering A→B remap...\n");
    if (!register_key_remap(KeyCode::A, KeyCode::B)) {
        printf("register_key_remap failed\n");
        stop_input_tracking();
        return 1;
    }
    printf("OK\n\n");

    // Step 2: Wait for physical A press
    printf("=== Waiting for physical key A to be pressed ===\n");
    printf("(A will be blocked — you won't see it typed)\n");
    printf("(Press 'y' after A to trigger flush)\n\n");

    while (!g_flush_requested.load()) {
        // Check if A was pressed
        if (g_key_a_down.load()) {
            // Wait for 'y' to trigger flush
            char ch = static_cast<char>(_getch());
            if (ch == 'y' || ch == 'Y') {
                g_flush_requested.store(true);
                printf("\n[TRIGGER] Flush requested...\n");

                // Flush the queued remapped keys
                flush_remap_output();
                printf("flush_remap_output() called\n");

                // Give the Logitech driver time to send B and input tracking to detect it
                printf("Detecting B (3 second window)...\n");
                for (int i = 0; i < 30; ++i) {
                    if (g_b_detected.load()) break;
                    std::this_thread::sleep_for(std::chrono::milliseconds(100));
                }
                break;
            }
            g_key_a_down.store(false); // reset for retry
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
    }

    // Step 3: Report result
    printf("\n=== RESULT ===\n");
    if (g_b_detected.load()) {
        printf("PASS — B was successfully sent after remapping A→B\n");
    } else {
        // Final check using get_pressed_keys
        auto keys = get_pressed_keys();
        bool found_b = false;
        for (auto k : keys) {
            if (k == 'B') { found_b = true; break; }
        }
        if (found_b) {
            printf("PASS — B is currently pressed (detected via get_pressed_keys)\n");
        } else {
            printf("FAIL — B was NOT detected\n");
        }
    }

    // Cleanup
    printf("\nCleaning up...\n");
    unregister_key_remap(KeyCode::A);
    clear_key_remaps();
    stop_input_tracking();
    printf("Done.\n");

    return 0;
}
