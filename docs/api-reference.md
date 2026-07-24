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

## Key Interception / Queue & Replay API

Temporarily blocks physical keyboard input during a critical section and replays queued keys after it ends. Uses a `WH_KEYBOARD_LL` low-level hook with a lock-free ring buffer — hook path is zero-lock, zero-alloc, suitable for ~1ms high-frequency triggering.

> **Important**: Only 3 API functions, no setup/teardown needed. Just wrap your critical section.

### API

| Function | Description |
|----------|-------------|
| `begin_key_intercept()` | Start intercepting physical keyboard input. All subsequent key presses are queued and **blocked** from reaching any application. Starts the low-level hook if not already running. |
| `end_key_intercept()` | Stop intercepting. **Replays** all queued keys (down then up for each event) in FIFO order, then resets the queue and stops the hook. |
| `discard_queued_keys()` | Stop intercepting and discard all queued keys without replaying them. Stops the hook. |

### Usage

```cpp
begin_key_intercept();
mouse_move_relative(dx, dy);
if (useSkills) SimulateKey(attack_skills);
key_press(kAttackKey);
Sleep(kAttackDelayMs);
mouse_move_relative(-dx, -dy);
end_key_intercept();
```

During the `begin_key_intercept()` / `end_key_intercept()` window, all physical keyboard input is queued. After `end_key_intercept()`, keys are replayed in order so the game never misses a press.

### Performance

- **Hook path**: 1× `atomic<bool> load(relaxed)` + 1× `atomic<int> load(relaxed)` + 1× `fetch_add` + struct write + `fetch_add(release)`. No mutex, no allocation, no syscall.
- **Ring buffer**: 512 entries pre-allocated, power-of-2 modulo.
- **Thread safety**: Release/acquire pairing guarantees the main thread sees fully-written entries when reading the queue.

---

## Mouse Interception / Queue & Replay API

Temporarily blocks physical mouse button and wheel input during a critical section and replays queued events after it ends. **Mouse movement is NOT intercepted** — the cursor moves freely during the entire interception window.

Uses a `WH_MOUSE_LL` low-level hook with a lock-free ring buffer — hook path is zero-lock, zero-alloc, suitable for ~1ms high-frequency triggering.

> **Note**: Only button clicks (down/up) and wheel events are intercepted. Mouse movement always passes through to applications.

### API

| Function | Description |
|----------|-------------|
| `begin_mouse_intercept()` | Start intercepting physical mouse button and wheel input. All subsequent button clicks and wheel scrolls are queued and **blocked** from reaching any application. Starts the low-level hook if not already running. |
| `end_mouse_intercept()` | Stop intercepting. **Replays** all queued events (button presses, releases, wheel scrolls) in FIFO order, then resets the queue and stops the hook. |
| `discard_queued_mouse_events()` | Stop intercepting and discard all queued events without replaying them. Stops the hook. |

### Usage

```cpp
begin_mouse_intercept();
// During this window, physical mouse clicks and wheel events are blocked.
// The cursor still moves normally.
// ... do your programmatic mouse operations ...
end_mouse_intercept();
```

### Events recorded

| Event | Queued as |
|-------|-----------|
| LEFT / RIGHT / MIDDLE button press | `DOWN` + button ID |
| LEFT / RIGHT / MIDDLE button release | `UP` + button ID |
| X1 / X2 button press | `DOWN` + button ID |
| X1 / X2 button release | `UP` + button ID |
| Wheel scroll | `WHEEL` + delta |

### Performance

- **Hook path**: identical design to keyboard intercept — 1× `atomic<bool> load(relaxed)` + 1× `atomic<int> load(relaxed)` + 1× `fetch_add` + struct write + `fetch_add(release)`. No mutex, no allocation, no syscall.
- **Ring buffer**: 4096 entries pre-allocated, power-of-2 modulo.
- **Thread safety**: Release/acquire pairing guarantees the main thread sees fully-written entries when reading the queue.

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
