// logitech.cpp
#include "pch.h"
#include "logitech.h"

namespace send {

static void set_bit(uint8_t& byte, uint8_t bit, bool value) {
    if (value) byte |= bit;
    else       byte &= ~bit;
}

// USB HID modifier bit for usage 0xE0-0xE7 indexed at offset 0-7
static uint8_t modifier_bit_for_usage(uint8_t usage) {
    constexpr uint8_t kModBits[] = {
        0x01, 0x10, 0x02, 0x20,  // LCtrl, RCtrl, LShift, RShift
        0x04, 0x40, 0x08, 0x80,  // LAlt,  RAlt,  LGui,   RGui
    };
    return kModBits[usage - 0xE0];
}

// Mouse button flags (same order as MouseButton enum values 1..5)
static constexpr uint8_t kButtonBits[] = {
    static_cast<uint8_t>(LogitechDriver::MouseButtonFlag::Left),
    static_cast<uint8_t>(LogitechDriver::MouseButtonFlag::Right),
    static_cast<uint8_t>(LogitechDriver::MouseButtonFlag::Middle),
    static_cast<uint8_t>(LogitechDriver::MouseButtonFlag::X1),
    static_cast<uint8_t>(LogitechDriver::MouseButtonFlag::X2),
};

// 构造函数：RAII初始化驱动
Logitech::Logitech() {
    driver_.create();
}

// 析构函数：RAII释放驱动
Logitech::~Logitech() {
    release_all_keys();
    release_all_mouse();
    driver_.destroy();
}

// 获取单例
Logitech& Logitech::get_logitech_instance() {
    static Logitech instance; // 延迟初始化，线程安全
    return instance;
}

// 发送鼠标报告（支持移动、滚轮、按键等事件）
bool Logitech::send_mouse_report(const MOUSEINPUT& mi) {
    std::lock_guard lock(mouse_mutex_);

    // 处理鼠标移动
    if (mi.dwFlags & MOUSEEVENTF_MOVE) {
        mouse_report_.x_ = mi.dx;
        mouse_report_.y_ = mi.dy;
    } else {
        mouse_report_.x_ = 0;
        mouse_report_.y_ = 0;
    }

    // 处理鼠标滚轮
    if (mi.dwFlags & MOUSEEVENTF_WHEEL) {
        mouse_report_.wheel_ = (static_cast<int32_t>(mi.mouseData) > 0) ? 1 : -1;
    }

    // 按键 — 通过位掩码更新 button_byte_
    auto apply = [&](DWORD down_flag, DWORD up_flag, uint8_t bit) {
        if (mi.dwFlags & (down_flag | up_flag))
            set_bit(mouse_report_.button_byte_, bit, mi.dwFlags & down_flag);
    };

    apply(MOUSEEVENTF_LEFTDOWN,   MOUSEEVENTF_LEFTUP,   kButtonBits[0]);
    apply(MOUSEEVENTF_RIGHTDOWN,  MOUSEEVENTF_RIGHTUP,  kButtonBits[1]);
    apply(MOUSEEVENTF_MIDDLEDOWN, MOUSEEVENTF_MIDDLEUP, kButtonBits[2]);

    if (mi.dwFlags & (MOUSEEVENTF_XDOWN | MOUSEEVENTF_XUP)) {
        bool down = (mi.dwFlags & MOUSEEVENTF_XDOWN);
        if (mi.mouseData & XBUTTON1) set_bit(mouse_report_.button_byte_, kButtonBits[3], down);
        if (mi.mouseData & XBUTTON2) set_bit(mouse_report_.button_byte_, kButtonBits[4], down);
    }

    return driver_.report_mouse(mouse_report_);
}

// 发送键盘输入事件（支持修饰键状态更新）
bool Logitech::send_keyboard_report(KeyCode vk, bool keydown) {
    std::lock_guard lock(keyboard_mutex_);

    auto usage = static_cast<uint8_t>(vk);

    // Modifier keys: USB usage IDs 0xE0-0xE7 → driver bitfield
    if (usage >= 0xE0 && usage <= 0xE7) {
        auto bit = modifier_bit_for_usage(usage);
        set_bit(keyboard_report_.modifiers_byte_, bit, keydown);
    } else {
        // Normal key: usage ID is the enum value itself
        if (keydown) {
            bool already_pressed = false;
            for (int i = 0; i < 6; i++) {
                if (keyboard_report_.keys_[i] == usage) {
                    already_pressed = true;
                    break;
                }
            }

            if (already_pressed) {
                return driver_.report_keyboard(keyboard_report_);
            }

            bool inserted = false;
            for (int i = 0; i < 6; i++) {
                if (keyboard_report_.keys_[i] == 0) {
                    keyboard_report_.keys_[i] = usage;
                    inserted = true;
                    break;
                }
            }

            if (!inserted) {
                printf("按键数量超过6个限制!\n");
                OutputDebugStringA("按键数量超过6个限制!\n");
            }
        } else {
            for (int i = 0; i < 6; i++) {
                if (keyboard_report_.keys_[i] == usage) {
                    keyboard_report_.keys_[i] = 0;
                    break;
                }
            }
        }
    }

    return driver_.report_keyboard(keyboard_report_);
}

//释放所有鼠标按键：清空鼠标 HID 报告并提交
void Logitech::release_all_mouse() {
    std::lock_guard lock(mouse_mutex_);

    mouse_report_.button_byte_ = 0;
    mouse_report_.x_ = 0;
    mouse_report_.y_ = 0;
    mouse_report_.wheel_ = 0;

    driver_.report_mouse(mouse_report_);
}

//释放所有按下的键：清空 HID 报告并提交
void Logitech::release_all_keys() {
    std::lock_guard lock(keyboard_mutex_);

    keyboard_report_.modifiers_byte_ = 0;
    memset(keyboard_report_.keys_, 0, sizeof(keyboard_report_.keys_));

    driver_.report_keyboard(keyboard_report_);
}

}
