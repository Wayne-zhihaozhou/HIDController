// virtual_keys.h — Virtual key code enum for HIDController
// Values are USB HID Keyboard Page Usage IDs (USB-IF defined).
// No conversion needed — the value IS the report sent to the driver.
#pragma once
#include <cstdint>

enum class KeyCode : uint16_t {
	// ---- Standard keys ----
	CANCEL    = 0x9B,
	BACK      = 0x2A,
	TAB       = 0x2B,
	ENTER     = 0x28,
	SHIFT     = 0xE1,	// LSHIFT (general shift)
	CTRL      = 0xE0,	// LCTRL (general ctrl)
	ALT       = 0xE2,	// LALT (general alt)
	PAUSE     = 0x48,
	CAPS_LOCK = 0x39,
	ESCAPE    = 0x29,

	// ---- Navigation ----
	SPACE     = 0x2C,
	PRIOR     = 0x4B,
	NEXT      = 0x4E,
	END       = 0x4D,
	HOME      = 0x4A,
	LEFT      = 0x50,
	UP        = 0x52,
	RIGHT     = 0x4F,
	DOWN      = 0x51,
	SNAPSHOT  = 0x46,
	INSERT    = 0x49,
	DEL       = 0x4C,

	// ---- 0-9 top row ----
	NUM0      = 0x27,
	NUM1      = 0x1E,
	NUM2      = 0x1F,
	NUM3      = 0x20,
	NUM4      = 0x21,
	NUM5      = 0x22,
	NUM6      = 0x23,
	NUM7      = 0x24,
	NUM8      = 0x25,
	NUM9      = 0x26,

	// ---- A-Z ----
	A         = 0x04,
	B         = 0x05,
	C         = 0x06,
	D         = 0x07,
	E         = 0x08,
	F         = 0x09,
	G         = 0x0A,
	H         = 0x0B,
	I         = 0x0C,
	J         = 0x0D,
	K         = 0x0E,
	L         = 0x0F,
	M         = 0x10,
	N         = 0x11,
	O         = 0x12,
	P         = 0x13,
	Q         = 0x14,
	R         = 0x15,
	S         = 0x16,
	T         = 0x17,
	U         = 0x18,
	V         = 0x19,
	W         = 0x1A,
	X         = 0x1B,
	Y         = 0x1C,
	Z         = 0x1D,

	// ---- Modifier / Special ----
	LWIN      = 0xE3,
	RWIN      = 0xE7,
	APPS      = 0x65,

	// ---- Numpad ----
	NUMPAD0   = 0x62,
	NUMPAD1   = 0x59,
	NUMPAD2   = 0x5A,
	NUMPAD3   = 0x5B,
	NUMPAD4   = 0x5C,
	NUMPAD5   = 0x5D,
	NUMPAD6   = 0x5E,
	NUMPAD7   = 0x5F,
	NUMPAD8   = 0x60,
	NUMPAD9   = 0x61,
	MULTIPLY  = 0x55,
	ADD       = 0x57,
	SUBTRACT  = 0x56,
	DECIMAL   = 0x63,
	DIVIDE    = 0x54,

	// ---- Function keys ----
	F1        = 0x3A,
	F2        = 0x3B,
	F3        = 0x3C,
	F4        = 0x3D,
	F5        = 0x3E,
	F6        = 0x3F,
	F7        = 0x40,
	F8        = 0x41,
	F9        = 0x42,
	F10       = 0x43,
	F11       = 0x44,
	F12       = 0x45,
	F13       = 0x68,
	F14       = 0x69,
	F15       = 0x6A,
	F16       = 0x6B,
	F17       = 0x6C,
	F18       = 0x6D,
	F19       = 0x6E,
	F20       = 0x6F,
	F21       = 0x70,
	F22       = 0x71,
	F23       = 0x72,
	F24       = 0x73,

	// ---- Status ----
	NUM_LOCK    = 0x53,
	SCROLL_LOCK = 0x47,

	// ---- Shift / Ctrl / Alt ----
	LSHIFT    = 0xE1,
	RSHIFT    = 0xE5,
	LCTRL     = 0xE0,
	RCTRL     = 0xE4,
	LALT      = 0xE2,
	RALT      = 0xE6,

	// ---- OEM ----
	OEM_1       = 0x33,
	OEM_PLUS    = 0x2E,
	OEM_COMMA   = 0x36,
	OEM_MINUS   = 0x2D,
	OEM_PERIOD  = 0x37,
	OEM_2       = 0x38,
	OEM_3       = 0x35,
	OEM_4       = 0x2F,
	OEM_5       = 0x31,
	OEM_6       = 0x30,
	OEM_7       = 0x34,
	OEM_102     = 0x64,
};
