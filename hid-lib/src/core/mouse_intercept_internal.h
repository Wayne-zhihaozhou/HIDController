// mouse_intercept_internal.h — Internal declarations for mouse interception module
// Allows ib_send_mouse.cpp to notify the interception module of programmatic sends.
#pragma once
#include <windows.h>

// Called before each Logitech driver mouse send so the LL hook can distinguish
// programmatic output from physical mouse input.
// msg_type: the WM_* message that the hook will see (WM_MOUSEMOVE, WM_*BUTTONDOWN, etc.)
void on_programmatic_mouse_send(DWORD msg_type);
