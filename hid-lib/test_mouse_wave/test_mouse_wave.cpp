// test_mouse_wave.cpp — Repeatedly oscillate mouse with relative moves
// Sequence: (50, 0), (-50, 0), (50, 0), (-50, 0), ...

#define _CRT_SECURE_NO_WARNINGS

#include <windows.h>
#include <stdio.h>
#include <thread>
#include <chrono>

#include "hid_controller.h"

// Override DLL import macros so we don't need the .lib linker trick
#pragma comment(lib, "hid_controller.lib")

int main() {
    printf("Mouse Wave Test\n");
    printf("Sequence: (50,0) -> (-50,0) -> (50,0) -> (-50,0) ...\n");
    printf("Press Ctrl+C to stop.\n\n");

    const int dx[] = { 50, -50 };
    const int dy[] = { 0, 0 };
    const int count = 2;

    int iter = 0;
    while (true) {
        for (int i = 0; i < count; i++) {
            mouse_move_relative(dx[i], dy[i]);
            printf("Iter %d: move_relative(%+d, %+d)\n", iter, dx[i], dy[i]);
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
        }
        iter++;
    }

    return 0;
}
