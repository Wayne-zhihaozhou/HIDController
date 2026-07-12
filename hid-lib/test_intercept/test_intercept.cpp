// Minimal test: A→B remap, flush in tight loop
#include <windows.h>
#include <stdio.h>
#include "hid_controller.h"

int main() {
    printf("=== A→B Remap Test (tight flush loop) ===\n");

    set_intercept_device(0);
    register_key_remap(KeyCode::A, KeyCode::B);
    printf("Remap A→B registered. Press A and hold.\n\n");

    while (true) {
        flush_remap_output();
    }
}
