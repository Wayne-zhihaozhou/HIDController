// test_hid_controller.cpp — Test executable for HID Controller output API
// Exercises all mouse and keyboard control functions exported from hid_controller.dll

#define _CRT_SECURE_NO_WARNINGS

#include <windows.h>
#include <stdio.h>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <thread>
#include <chrono>

#include "hid_controller.h"

// Override DLL import macros so we don't need the .lib linker trick
#pragma comment(lib, "hid_controller.lib")

// ==================== Test Cases ====================

static POINT get_cursor_pos() {
    POINT p;
    GetCursorPos(&p);
    return p;
}

static void print_banner() {
    printf("============================================\n");
    printf("  HID Controller Output Test\n");
    printf("============================================\n\n");
}

static void print_usage() {
    printf("Commands:\n");
    printf("  m 10 5          Move mouse relative by (10, 5)\n");
    printf("  M 500 300       Move mouse absolute to (500, 300)\n");
    printf("  ldn / lup       Left mouse down / up\n");
    printf("  ldc             Left mouse click\n");
    printf("  rdn / rup       Right mouse down / up\n");
    printf("  rdc             Right mouse click\n");
    printf("  w -3 / w 2      Scroll wheel (negative=up, positive=down)\n");
    printf("  coeff [val]     Set coefficient (default 2.0)\n");
    printf("  ac              Auto-calibrate\n");
    printf("  dac             Disable mouse acceleration\n");
    printf("  eac             Enable mouse acceleration\n");
    printf("  k NAME          Press key by name\n");
    printf("  kc NAME...      Hold keys (combo)\n");
    printf("  ks NAME...      Sequential keys\n");
    printf("  rel             Release all keys\n");
    printf("  sc              Show cursor position\n");
    printf("  t N             Run test N (1-9)\n");
    printf("  run             Run full sequence\n");
    printf("  all             Run ALL tests\n");
    printf("  help            Show this help\n");
    printf("  q               Quit\n");
    printf("\nKey names: A C V X Z SPACE ENTER LCTRL LSHIFT LWIN TAB ESCAPE F5 F12\n\n");
}

// ==================== Built-in Tests ====================

static void test_mouse_relative() {
    printf("--- test_mouse_relative ---\n");
    POINT before = get_cursor_pos();
    printf("Before: (%d, %d)\n", before.x, before.y);
    mouse_move_relative(50, 30);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    POINT after = get_cursor_pos();
    printf("After:  (%d, %d)  delta= (%d, %d)\n",
        after.x, after.y, after.x - before.x, after.y - before.y);
    printf("Expected: +50 x, +30 y\n\n");
}

static void test_mouse_absolute() {
    printf("--- test_mouse_absolute ---\n");
    printf("Moving absolute to (500, 300)...\n");
    mouse_move_absolute(500, 300);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    POINT cur = get_cursor_pos();
    printf("Current: (%d, %d), target: (500, 300)\n\n", cur.x, cur.y);
}

static void test_mouse_click() {
    printf("--- test_mouse_click ---\n");
    printf("Left click (3x)...\n");
    for (int i = 0; i < 3; i++) {
        printf("  Click %d/3\n", i + 1);
        mouse_click(MouseButton::LEFT);
        std::this_thread::sleep_for(std::chrono::milliseconds(300));
    }
    printf("Done.\n\n");
}

static void test_mouse_down_up() {
    printf("--- test_mouse_down_up ---\n");
    printf("Left down... up...\n");
    mouse_down(MouseButton::LEFT);
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    mouse_up(MouseButton::LEFT);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    printf("Right down... up...\n");
    mouse_down(MouseButton::RIGHT);
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    mouse_up(MouseButton::RIGHT);
    printf("Done.\n\n");
}

static void test_mouse_wheel() {
    printf("--- test_mouse_wheel ---\n");
    printf("Scroll down 5x, up 2x...\n");
    for (int i = 0; i < 5; i++) { mouse_wheel(3); std::this_thread::sleep_for(std::chrono::milliseconds(100)); }
    for (int i = 0; i < 2; i++) { mouse_wheel(-3); std::this_thread::sleep_for(std::chrono::milliseconds(100)); }
    printf("Done.\n\n");
}

static void test_coefficient() {
    printf("--- test_coefficient ---\n");
    float orig = get_mouse_move_coefficient();
    printf("Original: %.4f\n", orig);
    set_mouse_move_coefficient(2.0f);
    printf("Set 2.0, got: %.4f\n", get_mouse_move_coefficient());
    set_mouse_move_coefficient(10.0f);
    printf("Set 10.0, got: %.4f\n", get_mouse_move_coefficient());
    set_mouse_move_coefficient(orig);
    printf("Restored: %.4f\n\n", get_mouse_move_coefficient());
}

static void test_auto_calibrate() {
    printf("--- test_auto_calibrate ---\n");
    printf("Running auto_calibrate()...\n");
    auto_calibrate();
    printf("Coefficient: %.4f\n\n", get_mouse_move_coefficient());
}

static void test_acceleration_control() {
    printf("--- test_acceleration_control ---\n");
    printf("Disable... enable...\n");
    disable_mouse_acceleration();
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    enable_mouse_acceleration();
    printf("Done.\n\n");
}

static void test_key_press() {
    printf("--- test_key_press ---\n");
    printf("Pressing ENTER 3x...\n");
    for (int i = 0; i < 3; i++) {
        printf("  Press ENTER %d/3\n", i + 1);
        key_press(KeyCode::ENTER);
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }
    printf("Done.\n\n");
}

static void test_key_combo() {
    printf("--- test_key_combo ---\n");
    std::vector<KeyCode> keys_vec = { KeyCode::LCTRL, KeyCode::A };
    printf("Ctrl+A (combo)...\n");
    key_combo(keys_vec);
    std::this_thread::sleep_for(std::chrono::milliseconds(300));
    key_up(KeyCode::LCTRL);
    key_up(KeyCode::A);
    printf("Done.\n\n");
}

static void test_key_seq() {
    printf("--- test_key_seq (Ctrl+A -> Ctrl+C -> Ctrl+V) ---\n");
    std::vector<KeyCode> sel_vec = { KeyCode::LCTRL, KeyCode::A };
    key_seq(sel_vec);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    std::vector<KeyCode> cp_vec = { KeyCode::LCTRL, KeyCode::C };
    key_seq(cp_vec);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    std::vector<KeyCode> paste_vec = { KeyCode::LCTRL, KeyCode::V };
    key_seq(paste_vec);
    printf("Done.\n\n");
}

static void test_release_all() {
    printf("--- test_release_all ---\n");
    std::vector<KeyCode> held_vec = { KeyCode::A, KeyCode::LCTRL };
    printf("Hold A+Ctrl, then release all...\n");
    key_combo(held_vec);
    std::this_thread::sleep_for(std::chrono::milliseconds(300));
    release_all_keys();
    printf("Done.\n\n");
}

static void test_full_sequence() {
    printf("--- test_full_sequence ---\n");
    printf("1. Set coefficient to 5.0\n");
    set_mouse_move_coefficient(5.0f);
    printf("2. Move to screen center\n");
    RECT desktop;
    GetWindowRect(GetDesktopWindow(), &desktop);
    int cx = (desktop.right + desktop.left) / 2;
    int cy = (desktop.bottom + desktop.top) / 2;
    mouse_move_absolute(cx, cy);
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    printf("3. Triple-click at center\n");
    for (int i = 0; i < 3; i++) {
        mouse_click(MouseButton::LEFT);
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }
    printf("4. Scroll wheel 3 down, 3 up\n");
    for (int i = 0; i < 3; i++) { mouse_wheel(3); std::this_thread::sleep_for(std::chrono::milliseconds(100)); }
    for (int i = 0; i < 3; i++) { mouse_wheel(-3); std::this_thread::sleep_for(std::chrono::milliseconds(100)); }
    printf("5. Type 'HID' via key_seq\n");
    std::vector<KeyCode> hid_vec = { KeyCode::H, KeyCode::I, KeyCode::D };
    key_seq(hid_vec);
    printf("6. Release all\n");
    release_all_keys();
    printf("Full sequence complete.\n\n");
}

// ==================== Interactive Mode ====================

static KeyCode find_key(const std::string& name) {
    struct KeyMap { const char* name; KeyCode code; };
    static const KeyMap keys[] = {
        { "A", KeyCode::A }, { "C", KeyCode::C }, { "V", KeyCode::V },
        { "X", KeyCode::X }, { "Z", KeyCode::Z }, { "SPACE", KeyCode::SPACE },
        { "ENTER", KeyCode::ENTER }, { "LCTRL", KeyCode::LCTRL },
        { "LSHIFT", KeyCode::LSHIFT }, { "LWIN", KeyCode::LWIN },
        { "TAB", KeyCode::TAB }, { "ESCAPE", KeyCode::ESCAPE },
        { "F5", KeyCode::F5 }, { "F12", KeyCode::F12 },
    };
    for (auto& km : keys) {
        if (name == km.name) return km.code;
    }
    return (KeyCode)0;
}

static int run_interactive() {
    printf("=== Interactive Mode ===\n\n");

    char line[512];
    while (std::cin.getline(line, sizeof(line))) {
        std::string cmd = line;
        while (!cmd.empty() && (cmd.back() == '\r' || cmd.back() == '\n' || cmd.back() == ' '))
            cmd.pop_back();
        while (!cmd.empty() && (cmd.front() == ' ' || cmd.front() == '\t'))
            cmd.erase(cmd.begin());
        if (cmd.empty()) { printf("> "); continue; }

        std::vector<std::string> args;
        {
            std::istringstream iss(cmd);
            std::string tok;
            while (iss >> tok) args.push_back(tok);
        }

        const std::string& action = args[0];

        if (action == "q" || action == "quit") {
            break;
        } else if (action == "help") {
            print_usage();
        } else if (action == "m" && args.size() >= 3) {
            int dx = std::stoi(args[1]);
            int dy = std::stoi(args[2]);
            mouse_move_relative(dx, dy);
            printf("Relative move (+%d, +%d)\n", dx, dy);
        } else if (action == "M" && args.size() >= 3) {
            int x = std::stoi(args[1]);
            int y = std::stoi(args[2]);
            mouse_move_absolute(x, y);
            printf("Absolute move to (%d, %d)\n", x, y);
        } else if (action == "ldn") {
            mouse_down(MouseButton::LEFT);
            printf("Left mouse down\n");
        } else if (action == "lup") {
            mouse_up(MouseButton::LEFT);
            printf("Left mouse up\n");
        } else if (action == "ldc") {
            mouse_click(MouseButton::LEFT);
            printf("Left mouse click\n");
        } else if (action == "rdn") {
            mouse_down(MouseButton::RIGHT);
            printf("Right mouse down\n");
        } else if (action == "rup") {
            mouse_up(MouseButton::RIGHT);
            printf("Right mouse up\n");
        } else if (action == "rdc") {
            mouse_click(MouseButton::RIGHT);
            printf("Right mouse click\n");
        } else if (action == "w" && args.size() >= 2) {
            int delta = std::stoi(args[1]);
            mouse_wheel(delta);
            printf("Wheel: %d\n", delta);
        } else if (action == "coeff") {
            float val = args.size() >= 2 ? std::stof(args[1]) : 2.0f;
            set_mouse_move_coefficient(val);
            printf("Coefficient: %.4f\n", get_mouse_move_coefficient());
        } else if (action == "ac") {
            auto_calibrate();
            printf("Coefficient: %.4f\n", get_mouse_move_coefficient());
        } else if (action == "dac") {
            disable_mouse_acceleration();
            printf("Acceleration disabled\n");
        } else if (action == "eac") {
            enable_mouse_acceleration();
            printf("Acceleration enabled\n");
        } else if (action == "k" && args.size() >= 2) {
            KeyCode kc = find_key(args[1]);
            if ((int)kc == 0) {
                printf("Unknown key: %s\n", args[1].c_str());
            } else {
                key_press(kc);
                printf("key_press(0x%02X)\n", (unsigned)kc);
            }
        } else if (action == "kc" && args.size() >= 2) {
            std::vector<KeyCode> combo;
            for (size_t i = 1; i < args.size(); i++) {
                KeyCode kc = find_key(args[i]);
                if ((int)kc) combo.push_back(kc);
            }
            if (!combo.empty()) {
                key_combo(combo);
                printf("key_combo(%zu keys)\n", combo.size());
            }
        } else if (action == "ks" && args.size() >= 2) {
            std::vector<KeyCode> seq;
            for (size_t i = 1; i < args.size(); i++) {
                KeyCode kc = find_key(args[i]);
                if ((int)kc) seq.push_back(kc);
            }
            if (!seq.empty()) {
                key_seq(seq);
                printf("key_seq(%zu keys)\n", seq.size());
            }
        } else if (action == "rel") {
            release_all_keys();
            printf("All keys released\n");
        } else if (action == "sc") {
            POINT p = get_cursor_pos();
            printf("Cursor: (%d, %d)\n", p.x, p.y);
        } else if (action == "run") {
            test_full_sequence();
        } else if (action == "all") {
            test_mouse_relative();
            test_mouse_absolute();
            test_mouse_click();
            test_mouse_down_up();
            test_mouse_wheel();
            test_coefficient();
            test_auto_calibrate();
            test_acceleration_control();
            test_key_press();
            test_key_combo();
            test_key_seq();
            test_release_all();
            test_full_sequence();
        } else if (action == "t" && args.size() >= 2) {
            int t = std::stoi(args[1]);
            switch (t) {
                case 1: test_mouse_relative(); break;
                case 2: test_mouse_absolute(); break;
                case 3: test_mouse_click(); break;
                case 4: test_mouse_down_up(); break;
                case 5: test_mouse_wheel(); break;
                case 6: test_coefficient(); break;
                case 7: test_auto_calibrate(); break;
                case 8: test_acceleration_control(); break;
                case 9: test_full_sequence(); break;
                default: printf("Unknown test: %d (1-9)\n", t);
            }
        } else {
            printf("Unknown command: %s\n", action.c_str());
        }
        printf("> ");
    }
    return 0;
}

// ==================== Main ====================

int main() {
    print_banner();

    printf("DLL check: %s\n\n",
        GetModuleHandle(L"hid_controller.dll") ? "OK" : "WARNING");

    printf("=== Running Built-in Test Suite ===\n\n");
    test_mouse_relative();
    test_mouse_absolute();
    test_mouse_click();
    test_mouse_down_up();
    test_mouse_wheel();
    test_coefficient();
    test_auto_calibrate();
    test_acceleration_control();
    test_key_press();
    test_key_combo();
    test_key_seq();
    test_release_all();
    test_full_sequence();

    printf("=== Built-in tests complete ===\n\n");
    print_usage();
    printf("> ");
    run_interactive();

    printf("\nTest program exited.\n");
    return 0;
}
