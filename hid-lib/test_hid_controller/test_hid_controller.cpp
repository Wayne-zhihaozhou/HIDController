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
#include <cmath>

#pragma comment(lib, "hid_controller.lib")

extern "C" {
    bool WINAPI mouse_move_relative(int32_t dx, int32_t dy);
    bool WINAPI mouse_move_absolute(uint32_t x, uint32_t y);
    bool WINAPI mouse_down(MouseButton button);
    bool WINAPI mouse_up(MouseButton button);
    bool WINAPI mouse_click(MouseButton button);
    bool WINAPI mouse_wheel(int32_t movement);
    void WINAPI set_mouse_move_coefficient(float coefficient);
    float WINAPI get_mouse_move_coefficient();
    void WINAPI auto_calibrate();
    void WINAPI disable_mouse_acceleration();
    void WINAPI enable_mouse_acceleration();

    bool WINAPI key_down(KeyCode vk);
    bool WINAPI key_up(KeyCode vk);
    bool WINAPI key_press(KeyCode vk);
    void WINAPI release_all_keys();
}

extern "C++" {
    bool WINAPI key_combo(const std::vector<KeyCode>& keys);
    bool WINAPI key_seq(const std::vector<KeyCode>& keys);
}

enum class MouseButton : uint16_t {
    LEFT     = 1,
    RIGHT    = 2,
    MIDDLE   = 3,
    X1       = 4,
    X2       = 5,
};

enum class KeyCode : uint16_t {
    A         = 0x04,
    C         = 0x06,
    V         = 0x19,
    X         = 0x1B,
    Z         = 0x1D,
    SPACE     = 0x2C,
    ENTER     = 0x28,
    LCTRL     = 0xE0,
    LSHIFT    = 0xE1,
    LWIN      = 0xE3,
    TAB       = 0x2B,
    ESCAPE    = 0x29,
    F5        = 0x3E,
    F12       = 0x45,
};

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
    printf("  a10             Set coefficient to 10.0\n");
    printf("  ac              Auto-calibrate\n");
    printf("  dac             Disable mouse acceleration\n");
    printf("  eac             Enable mouse acceleration\n");
    printf("  k A             Press key A (single press)\n");
    printf("  k A V           Paste text via Ctrl+A, Ctrl+V (seq)\n");
    printf("  kc A C V        Hold Ctrl, press A+C+V combo\n");
    printf("  rel             Release all keys\n");
    printf("  sc            Screen coordinates (current cursor pos)\n");
    printf("  help            Show this help\n");
    printf("  q               Quit\n");
    printf("\nKey codes: A C V X Z SPACE ENTER LCTRL LSHIFT TAB ESCAPE F5 F12\n\n");
}

static const char* keycode_to_name(KeyCode code) {
    switch (code) {
        case KeyCode::A:      return "A";
        case KeyCode::C:      return "C";
        case KeyCode::V:      return "V";
        case KeyCode::X:      return "X";
        case KeyCode::Z:      return "Z";
        case KeyCode::SPACE:  return "SPACE";
        case KeyCode::ENTER:  return "ENTER";
        case KeyCode::LCTRL:  return "LCTRL";
        case KeyCode::LSHIFT: return "LSHIFT";
        case KeyCode::LWIN:   return "LWIN";
        case KeyCode::TAB:    return "TAB";
        case KeyCode::ESCAPE: return "ESCAPE";
        case KeyCode::F5:     return "F5";
        case KeyCode::F12:    return "F12";
        default:              return "?";
    }
}

static POINT get_cursor_pos() {
    POINT p;
    GetCursorPos(&p);
    return p;
}

// ==================== Test Cases ====================

static int test_mouse_relative() {
    printf("--- test_mouse_relative ---\n");
    POINT before = get_cursor_pos();
    printf("Before: (%d, %d)\n", before.x, before.y);

    mouse_move_relative(50, 30);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    POINT after = get_cursor_pos();
    printf("After:  (%d, %d)  delta= (%d, %d)\n",
        after.x, after.y, after.x - before.x, after.y - before.y);
    printf("Expected: +50 x, +30 y\n\n");
    return 0;
}

static int test_mouse_absolute() {
    printf("--- test_mouse_absolute ---\n");
    POINT p = { 500, 300 };
    printf("Moving absolute to (%d, %d)...\n", p.x, p.y);
    mouse_move_absolute(p.x, p.y);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    POINT cur = get_cursor_pos();
    printf("Current: (%d, %d), target: (%d, %d)\n\n", cur.x, cur.y, p.x, p.y);
    return 0;
}

static int test_mouse_click() {
    printf("--- test_mouse_click ---\n");
    printf("Left click (3x with delay)...\n");
    for (int i = 0; i < 3; i++) {
        printf("  Click %d/3\n", i + 1);
        mouse_click(MouseButton::LEFT);
        std::this_thread::sleep_for(std::chrono::milliseconds(300));
    }
    printf("Done.\n\n");
    return 0;
}

static int test_mouse_down_up() {
    printf("--- test_mouse_down_up ---\n");
    printf("Left down...\n");
    mouse_down(MouseButton::LEFT);
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    printf("Left up...\n");
    mouse_up(MouseButton::LEFT);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    printf("Right down...\n");
    mouse_down(MouseButton::RIGHT);
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    printf("Right up...\n");
    mouse_up(MouseButton::RIGHT);
    printf("Done.\n\n");
    return 0;
}

static int test_mouse_wheel() {
    printf("--- test_mouse_wheel ---\n");
    printf("Scroll down 5 times (3 clicks each)...\n");
    for (int i = 0; i < 5; i++) {
        mouse_wheel(3);  // typical wheel click = 3
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    printf("Scroll up 2 times...\n");
    for (int i = 0; i < 2; i++) {
        mouse_wheel(-3);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    printf("Done.\n\n");
    return 0;
}

static int test_coefficient() {
    printf("--- test_coefficient ---\n");
    float orig = get_mouse_move_coefficient();
    printf("Original coefficient: %.4f\n", orig);

    set_mouse_move_coefficient(2.0f);
    float val = get_mouse_move_coefficient();
    printf("Set to 2.0, got: %.4f\n", val);

    set_mouse_move_coefficient(10.0f);
    val = get_mouse_move_coefficient();
    printf("Set to 10.0, got: %.4f\n", val);

    set_mouse_move_coefficient(orig);
    printf("Restored to: %.4f\n\n", get_mouse_move_coefficient());
    return 0;
}

static int test_auto_calibrate() {
    printf("--- test_auto_calibrate ---\n");
    printf("Running auto_calibrate()...\n");
    auto_calibrate();
    float after = get_mouse_move_coefficient();
    printf("Coefficient after calibrate: %.4f\n\n", after);
    return 0;
}

static int test_acceleration_control() {
    printf("--- test_acceleration_control ---\n");
    printf("Disabling mouse acceleration...\n");
    disable_mouse_acceleration();
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    printf("Enabling mouse acceleration...\n");
    enable_mouse_acceleration();
    printf("Done.\n\n");
    return 0;
}

static int test_key_press() {
    printf("--- test_key_press ---\n");
    printf("Pressing Enter 3 times...\n");
    for (int i = 0; i < 3; i++) {
        printf("  Press ENTER %d/3\n", i + 1);
        key_press(KeyCode::ENTER);
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }
    printf("Done.\n\n");
    return 0;
}

static int test_key_combo() {
    printf("--- test_key_combo ---\n");
    printf("Ctrl+A (select all)...\n");
    std::vector<KeyCode> keys_vec = { KeyCode::LCTRL, KeyCode::A };
    key_combo(keys_vec);
    std::this_thread::sleep_for(std::chrono::milliseconds(300));
    key_up(KeyCode::LCTRL);
    key_up(KeyCode::A);
    printf("Done.\n\n");
    return 0;
}

static int test_key_seq() {
    printf("--- test_key_seq (Ctrl+A, Ctrl+C, Ctrl+V paste simulation) ---\n");
    std::vector<KeyCode> sel_vec = { KeyCode::LCTRL, KeyCode::A };
    key_seq(sel_vec);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    std::vector<KeyCode> cp_vec = { KeyCode::LCTRL, KeyCode::C };
    key_seq(cp_vec);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    std::vector<KeyCode> paste_vec = { KeyCode::LCTRL, KeyCode::V };
    key_seq(paste_vec);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    printf("Done.\n\n");
    return 0;
}

static int test_release_all() {
    printf("--- test_release_all ---\n");
    printf("Pressing A+B+Ctrl simultaneously...\n");
    KeyCode held[] = { KeyCode::A, KeyCode::LCTRL };
    key_combo(held, 2);
    std::this_thread::sleep_for(std::chrono::milliseconds(300));
    printf("Releasing all keys...\n");
    release_all_keys();
    printf("Done.\n\n");
    return 0;
}

static int test_full_sequence() {
    printf("--- test_full_sequence ---\n");
    printf("1. Set coefficient to 5.0\n");
    set_mouse_move_coefficient(5.0f);

    printf("2. Move to center of screen\n");
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
    KeyCode hid[] = { KeyCode::H, KeyCode::I, KeyCode::D };
    key_seq(hid, 3);

    printf("6. Release all\n");
    release_all_keys();

    printf("Full sequence complete.\n\n");
    return 0;
}

// ==================== Interactive Mode ====================

static bool is_valid_hex(const char* s) {
    if (!s || !*s) return false;
    for (; *s; s++) {
        if (!(*s >= '0' && *s <= '9') && !(*s >= 'a' && *s <= 'f') && !(*s >= 'A' && *s <= 'F'))
            return false;
    }
    return true;
}

static int run_interactive() {
    printf("=== Interactive Mode ===\n\n");

    // Preload known key codes for interactive input
    struct KeyMap { std::string name; KeyCode code; };
    KeyMap keys[] = {
        { "A", KeyCode::A }, { "C", KeyCode::C }, { "V", KeyCode::V },
        { "X", KeyCode::X }, { "Z", KeyCode::Z }, { "SPACE", KeyCode::SPACE },
        { "ENTER", KeyCode::ENTER }, { "LCTRL", KeyCode::LCTRL },
        { "LSHIFT", KeyCode::LSHIFT }, { "LWIN", KeyCode::LWIN },
        { "TAB", KeyCode::TAB }, { "ESCAPE", KeyCode::ESCAPE },
        { "F5", KeyCode::F5 }, { "F12", KeyCode::F12 },
    };

    char line[512];
    while (std::cin.getline(line, sizeof(line))) {
        // Trim
        std::string cmd = line;
        while (!cmd.empty() && (cmd.back() == '\r' || cmd.back() == '\n' || cmd.back() == ' '))
            cmd.pop_back();
        while (!cmd.empty() && (cmd.front() == ' ' || cmd.front() == '\t'))
            cmd.erase(cmd.begin());
        if (cmd.empty()) { printf("> "); continue; }

        // Split by space
        std::vector<std::string> args;
        {
            std::istringstream iss(cmd);
            std::string tok;
            while (iss >> tok) args.push_back(tok);
        }

        const char* action = args[0];

        if (action == std::string("q") || action == std::string("quit")) {
            break;
        } else if (action == std::string("help")) {
            print_usage();
        } else if (action == std::string("m") && args.size() >= 3) {
            int dx = std::stoi(args[1]);
            int dy = std::stoi(args[2]);
            printf("Relative move (+%d, +%d)\n", dx, dy);
            mouse_move_relative(dx, dy);
        } else if (action == std::string("M") && args.size() >= 3) {
            int x = std::stoi(args[1]);
            int y = std::stoi(args[2]);
            printf("Absolute move to (%d, %d)\n", x, y);
            mouse_move_absolute(x, y);
        } else if (action == std::string("ldn")) {
            printf("Left mouse down\n");
            mouse_down((MouseButton)1);
        } else if (action == std::string("lup")) {
            printf("Left mouse up\n");
            mouse_up((MouseButton)1);
        } else if (action == std::string("ldc")) {
            printf("Left mouse click\n");
            mouse_click((MouseButton)1);
        } else if (action == std::string("rdn")) {
            printf("Right mouse down\n");
            mouse_down((MouseButton)2);
        } else if (action == std::string("rup")) {
            printf("Right mouse up\n");
            mouse_up((MouseButton)2);
        } else if (action == std::string("rdc")) {
            printf("Right mouse click\n");
            mouse_click((MouseButton)2);
        } else if (action == std::string("w") && args.size() >= 2) {
            int delta = std::stoi(args[1]);
            printf("Wheel: %d\n", delta);
            mouse_wheel(delta);
        } else if (action == std::string("a10") || action == std::string("coeff")) {
            float val = args.size() >= 2 ? std::stof(args[1]) : 2.0f;
            printf("Setting coefficient to %.1f\n", val);
            set_mouse_move_coefficient(val);
        } else if (action == std::string("ac")) {
            printf("Auto-calibrating...\n");
            auto_calibrate();
        } else if (action == std::string("dac")) {
            printf("Disabling acceleration\n");
            disable_mouse_acceleration();
        } else if (action == std::string("eac")) {
            printf("Enabling acceleration\n");
            enable_mouse_acceleration();
        } else if (action == std::string("k") && args.size() >= 2) {
            // Find key by name
            KeyCode kc = (KeyCode)0;
            bool found = false;
            for (auto& km : keys) {
                if (args[1] == km.name) {
                    kc = km.code;
                    found = true;
                    break;
                }
            }
            if (!found) {
                printf("Unknown key: %s. Use names from help.\n", args[1].c_str());
            } else {
                printf("key_press(%s)\n", args[1].c_str());
                key_press(kc);
            }
        } else if (action == std::string("kc") && args.size() >= 2) {
            // Key combo: hold specified keys
            std::vector<KeyCode> combo;
            for (size_t i = 1; i < args.size(); i++) {
                for (auto& km : keys) {
                    if (args[i] == km.name) { combo.push_back(km.code); }
                }
            }
            if (!combo.empty()) {
                printf("key_combo(%zu keys)\n", combo.size());
                key_combo(combo.data(), combo.size());
            }
        } else if (action == std::string("ks") && args.size() >= 2) {
            // Key seq
            std::vector<KeyCode> seq;
            for (size_t i = 1; i < args.size(); i++) {
                for (auto& km : keys) {
                    if (args[i] == km.name) { seq.push_back(km.code); }
                }
            }
            if (!seq.empty()) {
                printf("key_seq(%zu keys)\n", seq.size());
                key_seq(seq.data(), seq.size());
            }
        } else if (action == std::string("rel")) {
            printf("Releasing all keys\n");
            release_all_keys();
        } else if (action == std::string("sc")) {
            POINT p = get_cursor_pos();
            printf("Cursor: (%d, %d)\n", p.x, p.y);
        } else if (action == std::string("run")) {
            printf("Running full test sequence...\n");
            test_full_sequence();
        } else if (action == std::string("all")) {
            printf("Running ALL tests...\n");
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
        } else if (action == std::string("t") && args.size() >= 2) {
            // Run test by number: t1..t9
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
            printf("Unknown command: %s. Type 'help' for commands.\n", action);
        }
        printf("> ");
    }
    return 0;
}

// ==================== Main ====================

int main() {
    print_banner();

    printf("Checking DLL availability...\n");
    HMODULE hMod = GetModuleHandle(L"hid_controller.dll");
    if (!hMod) {
        // Try loading from current directory, then system path
        wchar_t exePath[MAX_PATH];
        GetModuleFileNameW(nullptr, exePath, MAX_PATH);
        std::wstring dllPath = exePath;
        auto pos = dllPath.find_last_of(L"\\/");
        if (pos != std::wstring::npos) {
            dllPath = dllPath.substr(0, pos + 1) + L"hid_controller.dll";
        }

        hMod = LoadLibraryW(dllPath.c_str());
        if (!hMod) {
            // Try loading from parent directory (project root)
            dllPath = exePath;
            pos = dllPath.find_last_of(L"\\/");
            if (pos != std::wstring::npos) {
                dllPath = dllPath.substr(0, pos); // exe dir
                pos = dllPath.find_last_of(L"\\/");
                if (pos != std::wstring::npos) {
                    dllPath = dllPath.substr(0, pos + 1) + L"hid_controller.dll";
                }
            }
            hMod = LoadLibraryW(dllPath.c_str());
        }

        if (!hMod) {
            printf("FAILED: Cannot load hid_controller.dll\n");
            printf("Searched:\n");
            printf("  - %ls\n", exePath);
            printf("  - current directory\n");
            printf("  - parent directory\n");
            printf("  - system DLL search path\n");
            printf("\nMake sure hid_controller.dll is built and accessible.\n");
            return 1;
        }
        printf("DLL loaded successfully from: %ls\n\n", dllPath.c_str());
    } else {
        printf("DLL already loaded.\n\n");
    }

    // Run all built-in tests
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

    // Interactive mode
    printf("=== Built-in tests complete ===\n\n");
    print_usage();
    printf("> ");
    run_interactive();

    printf("\nTest program exited.\n");
    return 0;
}
