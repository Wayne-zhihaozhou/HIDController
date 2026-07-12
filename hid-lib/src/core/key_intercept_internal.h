// key_intercept_internal.h — Internal declarations for key interception module
// Allows ib_send_keyboard.cpp to notify the interception module of programmatic sends.
#pragma once
#include "../../include/virtual_keys.h"

// Called before each Logitech driver keyboard send so the LL hook can distinguish
// programmatic output from physical keyboard input.
void on_programmatic_keyboard_send(KeyCode vk);
