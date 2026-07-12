# HIDController API Reference

HIDController is a Windows DLL library for sending raw HID (Human Interface Device) mouse and keyboard input, and monitoring raw input devices. The library bypasses standard Windows input APIs to deliver direct hardware-level input reports.

---

## Quick Start

```cpp
#include "hid_controller.h"

// Mouse: move 100 units right, click left button
mouse_move_relative(100, 0);
mouse_click(MouseButton::LEFT);

// Keyboard: press A, release A
key_down(KeyCode::A);
key_up(KeyCode::A);

// Keyboard: type "hello" (press + release sequence)
key_seq({KeyCode::H, KeyCode::E, KeyCode::L, KeyCode::L, KeyCode::O});

// Keyboard: Ctrl+C combo
key_combo({KeyCode::LCTRL, KeyCode::C});
```

---

## Types

### `MouseButton` (enum class, uint16_t)

Defined in [mouse_button.h](hid-lib/include/mouse_button.h).

| Value       | Meaning  |
|-------------|----------|
| `LEFT`      | Left     |
| `RIGHT`     | Right    |
| `MIDDLE`    | Middle   |
| `X1`        | X Button 1 |
| `X2`        | X Button 2 |

### `KeyCode` (enum class, uint16_t)

Defined in [virtual_keys.h](hid-lib/include/virtual_keys.h). Values are USB HID Keyboard Page Usage IDs.

**Standard keys:** `BACK`, `TAB`, `ENTER`, `SHIFT`, `CTRL`, `ALT`, `PAUSE`, `CAPS_LOCK`, `ESCAPE`, `SPACE`

**Navigation:** `PRIOR` (Page Up), `NEXT` (Page Down), `END`, `HOME`, `LEFT`, `UP`, `RIGHT`, `DOWN`, `SNAPSHOT`, `INSERT`, `DEL`

**Alphanumeric:** `NUM0`–`NUM9`, `A`–`Z`

**Function keys:** `F1`–`F24`

**Numpad:** `NUMPAD0`–`NUMPAD9`, `MULTIPLY`, `ADD`, `SUBTRACT`, `DECIMAL`, `DIVIDE`

**Modifiers:** `LSHIFT`/`RSHIFT`, `LCTRL`/`RCTRL`, `LALT`/`RALT`, `LWIN`/`RWIN`, `APPS`

**Status:** `NUM_LOCK`, `SCROLL_LOCK`

**OEM:** `OEM_1` through `OEM_102`

---

## Mouse API

All functions return `bool` — `true` on success, `false` on failure.

| Function | Description |
|----------|-------------|
| `mouse_move_relative(int32_t dx, int32_t dy)` | Move the cursor by a relative offset. |
| `mouse_move_absolute(uint32_t x, uint32_t y)` | Move the cursor to an absolute screen position. |
| `mouse_down(MouseButton button)` | Press down the specified mouse button. |
| `mouse_up(MouseButton button)` | Release the specified mouse button. |
| `mouse_click(MouseButton button)` | Press and release the specified button (one-shot click). |
| `mouse_wheel(int32_t movement)` | Scroll the mouse wheel. Positive = scroll up. |

### Mouse acceleration control

| Function | Description |
|----------|-------------|
| `set_mouse_move_coefficient(float coefficient)` | Set the mouse movement scaling coefficient. |
| `get_mouse_move_coefficient()` | Get the current mouse movement coefficient. |
| `auto_calibrate()` | Auto-calibrate the mouse movement coefficient. |
| `disable_mouse_acceleration()` | Disable Windows mouse acceleration. |
| `enable_mouse_acceleration()` | Re-enable Windows mouse acceleration. |

---

## Keyboard API

| Function | Return | Description |
|----------|--------|-------------|
| `key_down(KeyCode vk)` | `bool` | Press down a single key. |
| `key_up(KeyCode vk)` | `bool` | Release a single key. |
| `key_press(KeyCode vk)` | `bool` | Press and release a single key (one-shot). |
| `key_combo(const std::vector<KeyCode>& keys)` | `bool` | Press multiple keys simultaneously (e.g., Ctrl+C). |
| `key_seq(const std::vector<KeyCode>& keys)` | `bool` | Type a sequence of keys in order (press then release each). |
| `release_all_keys()` | `void` | Release all currently pressed keys. |

---

## Key Interception / Remapping API

Intercepts physical keyboard input via a low-level `WH_KEYBOARD_LL` hook and optionally remaps keys or blocks them entirely. The hook distinguishes physical key presses from programmatic output (via the Logitech HID driver) using a timestamped ring buffer, so remapped output is never re-intercepted.

> **Important**: `WH_KEYBOARD_LL` hooks are system-global and cannot distinguish between different physical keyboards. All physical keyboards are affected when interception is active.

### Setup

| Function | Return | Description |
|----------|--------|-------------|
| `enable_key_intercept(bool enable)` | `void` | Start/stop the LL keyboard hook. The hook thread starts on first enable and stops only when both remap mode and forbid-all mode are disabled. |
| `set_intercept_device(uintptr_t device_handle)` | `bool` | Reserve the physical keyboard device handle (from `keyboard_callback`) for future per-device filtering. |

### Key Remapping

Map one key to another at the LL hook level. The original key is blocked from reaching any window; the target key is sent via the Logitech HID driver.

| Function | Return | Description |
|----------|--------|-------------|
| `register_key_remap(KeyCode from_key, KeyCode to_key)` | `bool` | Register a remap: intercept `from_key`, send `to_key`. Returns `false` if `from_key == to_key`. |
| `unregister_key_remap(KeyCode from_key)` | `bool` | Remove a previously registered remap. Returns `false` if no such mapping exists. |
| `clear_key_remaps()` | `void` | Remove all registered key remappings. |
| `enable_remap_output()` | `void` | Enable remap output. After calling this, intercepted keys emit their remapped target key. |
| `disable_remap_output()` | `void` | Disable remap output (default). Intercepted keys are still blocked, but the target key is **not** sent. Tracks key state so key-up is never sent without a prior key-down. |

**Example:**
```cpp
// Block physical A key, emit B instead
register_key_remap(KeyCode::A, KeyCode::B);
enable_key_intercept(true);

// Remap is registered but output is disabled by default — pressing A
// only blocks it, nothing is sent.

enable_remap_output();
// Now pressing A blocks A and sends B.

disable_remap_output();
// Pressing A blocks A again, no output sent.
```

### Forbid-All Mode

Block **all** physical key presses from reaching any application. Programmatic output via `key_down`/`key_up`/etc. is unaffected.

| Function | Return | Description |
|----------|--------|-------------|
| `start_forbid_keys(uintptr_t device_handle)` | `bool` | Start blocking all physical keyboard input. The hook thread starts automatically if not already running. |
| `stop_forbid_keys()` | `void` | Stop blocking physical keyboard input. The hook thread shuts down if no other mode is active. |

**Example:**
```cpp
// Block all physical keyboard input
start_forbid_keys(device_handle);

// The program can still send keys programmatically:
key_press(KeyCode::ENTER);  // works normally

// Restore physical keyboard input
stop_forbid_keys();
```

### Interception lifecycle

The LL hook thread is managed automatically:
- `enable_key_intercept(true)`, `start_forbid_keys()` → hook starts if not already running
- Both modes must be stopped before the hook thread exits
- When the hook stops, any still-held remapped keys are automatically released (`key_up`)

---

## Raw Input Monitoring API

Starts a background thread that listens for raw HID input events from connected devices (mice, keyboards) and delivers callbacks on the calling thread.

### Callback types

```cpp
typedef void (*mouse_move_callback)(uintptr_t device_handle, int32_t dx, int32_t dy);
typedef void (*mouse_button_callback)(uintptr_t device_handle, uint32_t button, bool is_down);
typedef void (*keyboard_callback)(uintptr_t device_handle, uint16_t vkey, bool is_down);
typedef void (*mouse_wheel_callback)(uintptr_t device_handle, int32_t wheel_delta, int32_t horizontal);
```

| Function | Description |
|----------|-------------|
| `start_input_tracking(mouse_move_callback, keyboard_callback, mouse_button_callback, mouse_wheel_callback)` | Start raw input monitoring. Pass `nullptr` for any callback you don't need. Returns `true` on success. |
| `stop_input_tracking()` | Stop raw input monitoring. Must be called to clean up the background thread. |
| `is_tracking()` | Returns `true` if monitoring is active. |
| `register_raw_input(HWND hwnd)` | Register a window to receive raw input messages. Required on some Windows versions. |
| `get_device_name(uintptr_t device_handle, wchar_t* name, uint32_t* name_length)` | Get the human-readable name of a raw input device. |

### Polling functions (C++ linkage)

```cpp
// Returns {delta_x, delta_y} of the latest mouse movement since last poll.
// When called without arguments (Python) or via the no-arg overload (C++),
// returns the combined delta from all devices. When a device_handle is provided,
// returns the delta for that specific device only.
std::pair<long, long> get_mouse_delta();
std::pair<long, long> get_mouse_delta(uintptr_t device_handle);

// Returns a vector of currently pressed virtual-key codes.
std::vector<uint16_t> get_pressed_keys();
```

---

## Logitech Driver API (internal)

Namespace `send`. These are internal components for sending reports via a Logitech-specific HID driver. They are not part of the public DLL export but are available to library consumers who link against the sources.

### `LogitechDriver`

| Member | Description |
|--------|-------------|
| `LogitechDriver()` | Constructor, initializes `device_` to `INVALID_HANDLE_VALUE`. |
| `~LogitechDriver()` | Destructor, calls `destroy()`. |
| `create()` | Open the Logitech device. Returns `true` on success. |
| `destroy()` | Close the device handle. |
| `report_mouse(const MouseReport& report)` | Send a mouse report via `DeviceIoControl`. |
| `report_keyboard(const KeyboardReport& report)` | Send a keyboard report via `DeviceIoControl`. |

**`MouseReport`** (5 bytes):

| Field | Type | Description |
|-------|------|-------------|
| `button_` / `button_byte_` | uint8_t | Button state (L/R/M/X1/X2 packed into bitfield). |
| `x_` | int8_t | X movement delta. |
| `y_` | int8_t | Y movement delta. |
| `wheel_` | int8_t | Wheel delta. |
| `unknown_t_` | int8_t | Reserved. |

**`KeyboardReport`** (8 bytes):

| Field | Type | Description |
|-------|------|-------------|
| `modifiers_` / `modifiers_byte_` | uint8_t | Modifier key state (LCtrl, LShift, LAlt, LGui, RCtrl, RShift, RAlt, RGui). |
| `reserved_` | uint8_t | Reserved. |
| `keys_[6]` | uint8_t[6] | Up to 6 simultaneously pressed keys. |

### `Logitech` (singleton)

| Static/Member | Description |
|---------------|-------------|
| `static Logitech& get_logitech_instance()` | Get the singleton instance. |
| `send_keyboard_report(KeyCode vk, bool down)` | Send a keyboard press or release. |
| `send_mouse_report(const MOUSEINPUT& mi)` | Send a mouse input report. |
| `release_all_mouse()` | Release all pressed mouse buttons. |
| `release_all_keys()` | Release all pressed keys. |

---

## Error Handling

All public DLL functions return `bool` — `true` indicates success, `false` indicates failure. On failure, call `GetLastError()` for extended error information.

---

## Build

The library is built with CMake + Visual Studio 2022 (C++20, release64 configuration only).

```bash
cmake -G "Visual Studio 17 2022" -A x64 -B build
cmake --build build --config release64
```

Output: `dll/HIDController.dll` + headers in `include/`.
