// test_raw_input.cpp — Test executable for RAW INPUT tracking API
// HIDController project — exercises all raw input tracking functions

// Suppress localtime deprecation warning
#define _CRT_SECURE_NO_WARNINGS

#include <windows.h>
#include <stdio.h>
#include <iostream>
#include <vector>
#include <string>
#include <thread>
#include <chrono>
#include <mutex>
#include <atomic>
#include <iomanip>
#include <sstream>

#pragma comment(lib, "hid_controller.lib")

// DLL function signatures
extern "C" {
    bool WINAPI start_input_tracking(
        void(__stdcall*)(uintptr_t, int32_t, int32_t),
        void(__stdcall*)(uintptr_t, uint16_t, bool),
        void(__stdcall*)(uintptr_t, uint32_t, bool),
        void(__stdcall*)(uintptr_t, int32_t, int32_t));
    void WINAPI stop_input_tracking();
    bool WINAPI is_tracking();
    bool WINAPI register_raw_input(HWND hwnd);
    bool WINAPI get_device_name(uintptr_t device_handle, wchar_t* name, uint32_t* name_length);
}
extern "C++" {
    std::pair<long, long> get_mouse_delta();
    std::pair<long, long> get_mouse_delta(uintptr_t device_handle);
    std::vector<uint16_t> get_pressed_keys();
}

// ==================== Statistics ====================

static std::atomic<int64_t> g_mouse_move_count{0};
static std::atomic<int64_t> g_keyboard_count{0};
static std::atomic<int64_t> g_mouse_button_count{0};
static std::atomic<int64_t> g_wheel_count{0};
static std::mutex g_device_mutex;
static std::vector<uintptr_t> g_known_devices;

// ==================== Callback Functions ====================

static std::string timestamp() {
    auto now = std::chrono::system_clock::now();
    auto t = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()) % 1000;
    std::ostringstream oss;
    oss << std::put_time(std::localtime(&t), "%H:%M:%S")
        << '.' << std::setfill('0') << std::setw(3) << ms.count();
    return oss.str();
}

static void print_device_info(uintptr_t handle) {
    std::lock_guard<std::mutex> lock(g_device_mutex);
    for (uintptr_t h : g_known_devices) {
        if (h == handle) return;
    }
    g_known_devices.push_back(handle);
    wchar_t name[512] = {};
    uint32_t len = 512;
    if (get_device_name(handle, name, &len)) {
        printf("[%s] New device: %S\n", timestamp().c_str(), name);
    } else {
        printf("[%s] New device handle: 0x%Ix\n", timestamp().c_str(), handle);
    }
}

static void __stdcall on_mouse_move(uintptr_t device_handle, int32_t dx, int32_t dy) {
    if (dx != 0 || dy != 0) {
        g_mouse_move_count++;
        print_device_info(device_handle);
        printf("[%s] MOUSE_MOVE  handle=0x%Ix  dx=%d  dy=%d\n",
            timestamp().c_str(), device_handle, dx, dy);
    }
}

static void __stdcall on_keyboard(uintptr_t device_handle, uint16_t vkey, bool is_down) {
    g_keyboard_count++;
    print_device_info(device_handle);
    const char* action = is_down ? "DOWN" : "UP";
    printf("[%s] KEYBOARD  handle=0x%Ix  vkey=0x%02X (%d)  %s\n",
        timestamp().c_str(), device_handle, vkey, vkey, action);
}

static void __stdcall on_mouse_button(uintptr_t device_handle, uint32_t button, bool is_down) {
    g_mouse_button_count++;
    const char* btn_name = "?";
    switch (button) {
        case 1: btn_name = "LEFT";  break;
        case 2: btn_name = "RIGHT"; break;
        case 3: btn_name = "MIDDLE"; break;
        case 4: btn_name = "X1";    break;
        case 5: btn_name = "X2";    break;
    }
    const char* action = is_down ? "DOWN" : "UP";
    printf("[%s] MOUSE_BTN   handle=0x%Ix  button=%s(%u)  %s\n",
        timestamp().c_str(), device_handle, btn_name, button, action);
}

static void __stdcall on_wheel(uintptr_t device_handle, int32_t wheel_delta, int32_t horizontal) {
    g_wheel_count++;
    const char* axis = horizontal ? "HWHEEL" : "WHEEL";
    printf("[%s] WHEEL       handle=0x%Ix  %s_delta=%d\n",
        timestamp().c_str(), device_handle, axis, wheel_delta);
}

// ==================== Enumerate Connected Devices ====================

static void enumerate_devices() {
    printf("\n=== Connected HID Devices ===\n");
    UINT dev_count = 0;
    if (GetRawInputDeviceList(nullptr, &dev_count, sizeof(RAWINPUTDEVICELIST)) != 0) {
        printf("  (unable to enumerate: %lu)\n", GetLastError());
        return;
    }
    if (dev_count == 0) {
        printf("  (no devices)\n");
        return;
    }
    auto list = std::make_unique<RAWINPUTDEVICELIST[]>(dev_count);
    if (GetRawInputDeviceList(list.get(), &dev_count, sizeof(RAWINPUTDEVICELIST)) == (UINT)-1) {
        printf("  (enum failed: %lu)\n", GetLastError());
        return;
    }
    for (UINT i = 0; i < dev_count; i++) {
        wchar_t name[512] = {};
        uint32_t len = 512;
        get_device_name((uintptr_t)list[i].hDevice, name, &len);

        RID_DEVICE_INFO di = {};
        di.cbSize = sizeof(di);
        UINT size = sizeof(di);
        GetRawInputDeviceInfoA(list[i].hDevice, RIDI_DEVICEINFO, &di, &size);

        if (di.dwType == RIM_TYPEMOUSE) {
            printf("  [%u] %S  type=mouse\n", i, name);
        } else if (di.dwType == RIM_TYPEKEYBOARD) {
            printf("  [%u] %S  type=keyboard\n", i, name);
        } else if (di.dwType == RIM_TYPEHID) {
            printf("  [%u] %S  type=hid  vendor=0x%04X  product=0x%04X\n",
                i, name, di.hid.dwVendorId, di.hid.dwProductId);
        } else {
            printf("  [%u] %S  type=0x%04X\n", i, name, di.dwType);
        }
    }
    printf("=== %u device(s) ===\n\n", dev_count);
}

// ==================== Usage Help ====================

static void print_usage() {
    printf("\n");
    printf("============================================\n");
    printf("  HIDController RAW INPUT Test Program\n");
    printf("============================================\n");
    printf("Commands:\n");
    printf("  q           Quit the program\n");
    printf("  delta       Print accumulated mouse delta\n");
    printf("  delta N     Print accumulated delta for device handle N (hex)\n");
    printf("  keys        Print currently pressed keys\n");
    printf("  devices     Enumerate connected HID devices\n");
    printf("  status      Check tracking status\n");
    printf("  stats       Print event statistics\n");
    printf("  help        Show this help message\n");
    printf("\n");
    printf("Just move the mouse and press keys to see events!\n");
    printf("============================================\n\n");
}

// ==================== Main ====================

int main() {
    HINSTANCE hInstance = GetModuleHandle(nullptr);
    if (!hInstance) hInstance = LoadLibrary(L"kernel32.dll");

    print_usage();

    // Create a hidden window for the test program to receive WM_INPUT
    const wchar_t* className = L"HIDTestWindowClass";
    WNDCLASSW wc = {};
    wc.lpfnWndProc = DefWindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = className;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    RegisterClassW(&wc);

    HWND test_hwnd = CreateWindowExW(
        0, className, L"HIDTestWindow",
        WS_OVERLAPPEDWINDOW, 0, 0, 0, 0,
        nullptr, nullptr, hInstance, nullptr);

    if (!test_hwnd) {
        fprintf(stderr, "Failed to create test window. Error: %lu\n", GetLastError());
        return 1;
    }

    printf("Test window created: 0x%p\n", test_hwnd);

    // Register this window as a raw input sink (mouse + keyboard)
    if (!register_raw_input(test_hwnd)) {
        fprintf(stderr, "Failed to register raw input. Error: %lu\n", GetLastError());
        DestroyWindow(test_hwnd);
        return 1;
    }
    printf("Raw input registered for test window\n");

    // Start the input tracking DLL
    printf("\nStarting input tracking...\n");
    bool started = start_input_tracking(on_mouse_move, on_keyboard, on_mouse_button, on_wheel);
    if (!started) {
        fprintf(stderr, "Failed to start input tracking\n");
        DestroyWindow(test_hwnd);
        return 1;
    }
    printf("Input tracking started\n");

    printf("\nMove your mouse and press keys to see events!\n\n");

    // Run message loop in a separate thread so the main thread can read keyboard commands
    std::atomic<bool> running{true};
    std::thread msg_thread([&]() {
        MSG msg;
        while (running.load()) {
            if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
                if (msg.message == WM_QUIT) break;
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            } else {
                Sleep(1);
            }
        }
        // Process any remaining messages
        while (GetMessage(&msg, nullptr, 0, 0)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    });

    // Main thread: read keyboard commands
    char line[256];
    std::cout << "> ";
    std::cout.flush();

    while (std::cin.getline(line, sizeof(line))) {
        std::string cmd = line;
        // Trim trailing whitespace and CR
        while (!cmd.empty() && (cmd.back() == '\r' || cmd.back() == '\n' || cmd.back() == ' '))
            cmd.pop_back();
        while (!cmd.empty() && (cmd.front() == ' ' || cmd.front() == '\t'))
            cmd.erase(cmd.begin());

        if (cmd == "q" || cmd == "quit" || cmd == "exit") {
            break;
        } else if (cmd == "delta") {
            auto [dx, dy] = get_mouse_delta();
            printf("Accumulated delta: dx=%ld  dy=%ld\n\n", dx, dy);
        } else if (cmd.size() >= 6 && cmd.substr(0, 5) == "delta" && cmd[5] == ' ') {
            uintptr_t handle;
            if (sscanf_s(cmd.c_str()+6, " %Ix", &handle) == 1) {
                auto [dx, dy] = get_mouse_delta(handle);
                printf("Per-device delta [0x%Ix]: dx=%ld  dy=%ld\n\n", handle, dx, dy);
            } else {
                printf("Usage: delta 0x103f7\n\n");
            }
        } else if (cmd == "keys") {
            auto keys = get_pressed_keys();
            if (keys.empty()) {
                printf("No keys currently pressed\n");
            } else {
                printf("Pressed keys: ");
                for (size_t i = 0; i < keys.size(); i++) {
                    if (i > 0) printf(", ");
                    printf("0x%02X(%d)", keys[i], keys[i]);
                }
                printf("\n");
            }
            printf("\n");
        } else if (cmd == "devices") {
            enumerate_devices();
        } else if (cmd == "status") {
            printf("Tracking active: %s\n", is_tracking() ? "YES" : "NO");
            printf("\n");
        } else if (cmd == "stats") {
            printf("\n=== Event Statistics ===\n");
            printf("  Mouse moves:       %ld\n", (long)g_mouse_move_count.load());
            printf("  Keyboard events:   %ld\n", (long)g_keyboard_count.load());
            printf("  Mouse buttons:     %ld\n", (long)g_mouse_button_count.load());
            printf("  Wheel events:      %ld\n", (long)g_wheel_count.load());
            printf("  Total events:      %ld\n",
                (long)g_mouse_move_count.load() +
                (long)g_keyboard_count.load() +
                (long)g_mouse_button_count.load() +
                (long)g_wheel_count.load());
            {
                std::lock_guard<std::mutex> lock(g_device_mutex);
                printf("  Unique devices:    %zu\n", g_known_devices.size());
            }
            printf("========================\n\n");
        } else if (cmd.empty() || cmd == "help") {
            print_usage();
        } else {
            printf("Unknown command: '%s'. Type 'help' for commands.\n\n", cmd.c_str());
        }

        std::cout << "> ";
        std::cout.flush();
    }

    // Cleanup
    std::cout << "\nStopping input tracking...\n";
    running.store(false);
    msg_thread.join();

    stop_input_tracking();

    print_usage();

    // Final statistics
    printf("\n=== Final Statistics ===\n");
    printf("  Mouse moves:       %ld\n", (long)g_mouse_move_count.load());
    printf("  Keyboard events:   %ld\n", (long)g_keyboard_count.load());
    printf("  Mouse buttons:     %ld\n", (long)g_mouse_button_count.load());
    printf("  Wheel events:      %ld\n", (long)g_wheel_count.load());
    printf("  Total events:      %ld\n",
        (long)g_mouse_move_count.load() +
        (long)g_keyboard_count.load() +
        (long)g_mouse_button_count.load() +
        (long)g_wheel_count.load());
    {
        std::lock_guard<std::mutex> lock(g_device_mutex);
        printf("  Unique devices:    %zu\n", g_known_devices.size());
    }
    printf("========================\n");

    DestroyWindow(test_hwnd);
    UnregisterClassW(className, hInstance);

    printf("\nTest program exited.\n");
    return 0;
}
