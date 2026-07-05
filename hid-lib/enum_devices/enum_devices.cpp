// enum_devices.cpp -- Enumerate all connected HID device handles
// HIDController project -- uses raw Windows API, no hid_controller dependency

#include <windows.h>
#include <stdio.h>
#include <memory>

static void print_banner() {
    printf("============================================\n");
    printf("  HID Device Handle Enumerator\n");
    printf("============================================\n\n");
}

static const char* type_to_string(ULONG type) {
    switch (type) {
        case RIM_TYPEMOUSE:  return "MOUSE";
        case RIM_TYPEKEYBOARD: return "KEYBOARD";
        case RIM_TYPEHID:    return "HID";
        default:             return "UNKNOWN";
    }
}

static const char* mouse_usage_page_name(USHORT page) {
    switch (page) {
        case 0x01: return "Generic Desktop";
        case 0x0C: return "Consumer";
        default:   return NULL;
    }
}

static const char* mouse_usage_name(USHORT usage, USHORT page) {
    if (page == 0x01) {
        switch (usage) {
            case 0x01: return "Pointer";
            case 0x02: return "Mouse";
            case 0x04: return "Keyboard";
            case 0x05: return "Keypad";
            case 0x06: return "Multi-axis Controller";
            default:   break;
        }
    }
    if (page == 0x0C) {
        switch (usage) {
            case 0x0001: return "Consumer Control";
            case 0x00B5: return "Scan Next";
            case 0x00B6: return "Scan Previous";
            case 0x0221: return "Eject";
            case 0x0223: return "Stop";
            case 0x040D: return "Rewind";
            case 0x0409: return "Fast Forward";
            default:     break;
        }
    }
    return NULL;
}

static void print_device_info(UINT index, RAWINPUTDEVICELIST* dev) {
    printf("  [%u]  handle=0x%Ix\n", index, (uintptr_t)dev->hDevice);

    // Query device name
    wchar_t name[512] = {};
    UINT name_len = 512;
    if (GetRawInputDeviceInfoW(dev->hDevice, RIDI_DEVICENAME, name, &name_len) != (UINT)-1) {
        if (name_len > 0) {
            wprintf(L"         name=%s\n", name);
        }
    }

    RID_DEVICE_INFO rid = {};
    rid.cbSize = sizeof(RID_DEVICE_INFO);
    UINT data_size = sizeof(RID_DEVICE_INFO);
    if (GetRawInputDeviceInfoA(dev->hDevice, RIDI_DEVICEINFO, &rid, &data_size) == (UINT)-1) {
        printf("         (query failed: %lu)\n", GetLastError());
        printf("\n");
        return;
    }

    printf("         type=%s\n", type_to_string(dev->dwType));

    if (dev->dwType == RIM_TYPEMOUSE) {
        // RID_DEVICE_INFO_MOUSE: dwId, dwNumberOfButtons, dwSampleRate, fHasHorizontalWheel
        printf("         buttons=%u  sampleRate=%u  hScroll=%d\n",
            rid.mouse.dwNumberOfButtons, rid.mouse.dwSampleRate,
            rid.mouse.fHasHorizontalWheel);
        printf("         id=%u\n", rid.mouse.dwId);
    } else if (dev->dwType == RIM_TYPEKEYBOARD) {
        // RID_DEVICE_INFO_KEYBOARD: dwType, dwSubType, dwKeyboardMode, dwNumberOfFunctionKeys, dwNumberOfIndicators, dwNumberOfKeysTotal
        printf("         type=0x%04X  subType=0x%04X  mode=0x%04X  funcKeys=%u  indicators=%u  keysTotal=%u\n",
            rid.keyboard.dwType, rid.keyboard.dwSubType, rid.keyboard.dwKeyboardMode,
            rid.keyboard.dwNumberOfFunctionKeys, rid.keyboard.dwNumberOfIndicators,
            rid.keyboard.dwNumberOfKeysTotal);
    } else if (dev->dwType == RIM_TYPEHID) {
        // RID_DEVICE_INFO_HID: dwVendorId, dwProductId, dwVersionNumber, usUsagePage, usUsage
        printf("         vendor=0x%04X  product=0x%04X  version=0x%04X\n",
            rid.hid.dwVendorId, rid.hid.dwProductId, rid.hid.dwVersionNumber);
        printf("         usagePage=0x%04X  usage=0x%04X\n",
            rid.hid.usUsagePage, rid.hid.usUsage);
    }
    printf("\n");
}

int main() {
    print_banner();

    // Step 1: Query required array size
    UINT dev_count = 0;
    if (GetRawInputDeviceList(nullptr, &dev_count, sizeof(RAWINPUTDEVICELIST)) == (UINT)-1) {
        fprintf(stderr, "GetRawInputDeviceList (size query) failed: %lu\n", GetLastError());
        printf("\nPress Enter to exit...");
        getchar();
        return 1;
    }

    if (dev_count == 0) {
        printf("  (no raw input devices found)\n");
        printf("\nPress Enter to exit...");
        getchar();
        return 0;
    }

    // Step 2: Allocate exact size and get the list
    auto list = std::make_unique<RAWINPUTDEVICELIST[]>(dev_count);
    UINT list_size = dev_count;
    if (GetRawInputDeviceList(list.get(), &list_size, sizeof(RAWINPUTDEVICELIST)) == (UINT)-1) {
        fprintf(stderr, "GetRawInputDeviceList (get list) failed: %lu\n", GetLastError());
        printf("\nPress Enter to exit...");
        getchar();
        return 1;
    }

    printf("  Found %u raw input device(s):\n\n", list_size);

    for (UINT i = 0; i < list_size; i++) {
        print_device_info(i, &list[i]);
    }

    printf("=== %u device(s) ===\n", list_size);

    printf("\nPress Enter to exit...");
    getchar();

    return 0;
}
