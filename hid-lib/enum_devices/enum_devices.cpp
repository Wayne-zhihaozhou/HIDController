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
        // RID_DEVICE_INFO_MOUSE: dwButtons, dwPointers, dwFunctionKeys, dwButtonRate, dwClickPolicy,
        //   dwMouseID, dwNumberOfRawButtons, dwFirstButtonIndex
        printf("         buttons=%u  buttonsPerPointer=%u\n",
            rid.mouse.dwButtons, rid.mouse.dwNumberOfRawButtons);
        printf("         mouseID=%u  firstButtonIndex=%u\n",
            rid.mouse.dwMouseID, rid.mouse.dwFirstButtonIndex);
    } else if (dev->dwType == RIM_TYPEKEYBOARD) {
        // RID_DEVICE_INFO_KEYBOARD: dwType, dwInterfaceType, dwKeys, dwExtraInfo
        printf("         keyboardType=0x%04X  interfaceType=0x%08X  keys=%u\n",
            rid.keyboard.dwType, rid.keyboard.dwInterfaceType, rid.keyboard.dwKeys);
    } else if (dev->dwType == RIM_TYPEHID) {
        // RID_DEVICE_INFO_HID: dwVendorId, dwProductId, dwVersionNumber,
        //   dwUsagePage, dwUsage, dwNumberOfKeys, dwNumberOfInputBuffers
        printf("         vendor=0x%04X  product=0x%04X  version=0x%04X\n",
            rid.hid.dwVendorId, rid.hid.dwProductId, rid.hid.dwVersionNumber);
        printf("         usagePage=0x%04X  usage=0x%04X\n",
            rid.hid.dwUsagePage, rid.hid.dwUsage);
        printf("         numKeys=%u  numInputBuffers=%u\n",
            rid.hid.dwNumberOfKeys, rid.hid.dwNumberOfInputBuffers);
    }
    printf("\n");
}

int main() {
    print_banner();

    UINT dev_count = 0;
    auto list = std::make_unique<RAWINPUTDEVICELIST[]>(256);

    if (GetRawInputDeviceList(list.get(), &dev_count, sizeof(RAWINPUTDEVICELIST)) == (UINT)-1) {
        fprintf(stderr, "GetRawInputDeviceList failed: %lu\n", GetLastError());
        return 1;
    }

    if (dev_count == 0) {
        printf("  (no raw input devices found)\n");
        return 0;
    }

    printf("  Found %u raw input device(s):\n\n", dev_count);

    for (UINT i = 0; i < dev_count; i++) {
        print_device_info(i, &list[i]);
    }

    printf("=== %u device(s) ===\n", dev_count);
    return 0;
}
