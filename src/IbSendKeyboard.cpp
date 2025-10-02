//ibsendkeyboard.cpp
#include"pch.h"


// 使用扫描码发送键盘事件，避免 SYSKey
VOID WINAPI IbSend_keybd_event(
    _In_ uint16_t scancode,
    _In_ DWORD dwFlags,
    _In_ ULONG_PTR dwExtraInfo
) {
    INPUT input = {};
    input.type = INPUT_KEYBOARD;
    input.ki.wVk = 0;               // 使用扫描码
    input.ki.wScan = scancode;
    input.ki.dwFlags = dwFlags | KEYEVENTF_SCANCODE;
    input.ki.dwExtraInfo = dwExtraInfo;
    SendInput(1, &input, sizeof(INPUT));
}

// 按下/释放指定虚拟键 + 修饰键
bool WINAPI IbSendKeybdDownUp(uint16_t vk, uint16_t scancode, Send::KeyboardModifiers modifiers) {
    struct ModKey { uint16_t vk; uint16_t scancode; bool active; };
    ModKey modKeys[] = {
        { VK_LCONTROL, MapVirtualKey(VK_LCONTROL, MAPVK_VK_TO_VSC), modifiers.LCtrl },
        { VK_RCONTROL, MapVirtualKey(VK_RCONTROL, MAPVK_VK_TO_VSC), modifiers.RCtrl },
        { VK_LSHIFT,   MapVirtualKey(VK_LSHIFT, MAPVK_VK_TO_VSC),   modifiers.LShift },
        { VK_RSHIFT,   MapVirtualKey(VK_RSHIFT, MAPVK_VK_TO_VSC),   modifiers.RShift },
        { VK_LMENU,    MapVirtualKey(VK_LMENU, MAPVK_VK_TO_VSC),    modifiers.LAlt },
        { VK_RMENU,    MapVirtualKey(VK_RMENU, MAPVK_VK_TO_VSC),    modifiers.RAlt },
        { VK_LWIN,     MapVirtualKey(VK_LWIN, MAPVK_VK_TO_VSC),     modifiers.LWin },
        { VK_RWIN,     MapVirtualKey(VK_RWIN, MAPVK_VK_TO_VSC),     modifiers.RWin }
    };

    std::vector<INPUT> inputs;

    auto pressKey = [&](uint16_t sc) {
        INPUT in = {};
        in.type = INPUT_KEYBOARD;
        in.ki.wVk = 0;
        in.ki.wScan = sc;
        inputs.push_back(in);
        };

    auto releaseKey = [&](uint16_t sc) {
        INPUT in = {};
        in.type = INPUT_KEYBOARD;
        in.ki.wVk = 0;
        in.ki.wScan = sc;
        in.ki.dwFlags = KEYEVENTF_KEYUP | KEYEVENTF_SCANCODE;
        inputs.push_back(in);
        };

    // 按下修饰键
    for (auto& mk : modKeys)
        if (mk.active) pressKey(mk.scancode);

    // 按下目标键
    pressKey(scancode);
    releaseKey(scancode);

    // 释放修饰键（逆序）
    for (int idx = sizeof(modKeys) / sizeof(ModKey) - 1; idx >= 0; idx--)
        if (modKeys[idx].active) releaseKey(modKeys[idx].scancode);

    // 发送所有事件
    return SendInput(static_cast<UINT>(inputs.size()), inputs.data(), sizeof(INPUT)) == inputs.size();
}

// 字符到虚拟键和扫描码映射
// 字符到虚拟键和扫描码映射
Send::CharMapping CharToVK(char ch) {
    if (ch >= 'A' && ch <= 'Z')
        return { static_cast<uint16_t>(ch), true, static_cast<uint16_t>(MapVirtualKey(ch, MAPVK_VK_TO_VSC)) };
    if (ch >= 'a' && ch <= 'z')
        return { static_cast<uint16_t>(toupper(ch)), false, static_cast<uint16_t>(MapVirtualKey(toupper(ch), MAPVK_VK_TO_VSC)) };
    if (ch >= '0' && ch <= '9')
        return { static_cast<uint16_t>(ch), false, static_cast<uint16_t>(MapVirtualKey(ch, MAPVK_VK_TO_VSC)) };

    switch (ch) {
    case '!': return { '1', true, static_cast<uint16_t>(MapVirtualKey('1', MAPVK_VK_TO_VSC)) };
    case '@': return { '2', true, static_cast<uint16_t>(MapVirtualKey('2', MAPVK_VK_TO_VSC)) };
    case '#': return { '3', true, static_cast<uint16_t>(MapVirtualKey('3', MAPVK_VK_TO_VSC)) };
    case '$': return { '4', true, static_cast<uint16_t>(MapVirtualKey('4', MAPVK_VK_TO_VSC)) };
    case '%': return { '5', true, static_cast<uint16_t>(MapVirtualKey('5', MAPVK_VK_TO_VSC)) };
    case '^': return { '6', true, static_cast<uint16_t>(MapVirtualKey('6', MAPVK_VK_TO_VSC)) };
    case '&': return { '7', true, static_cast<uint16_t>(MapVirtualKey('7', MAPVK_VK_TO_VSC)) };
    case '*': return { '8', true, static_cast<uint16_t>(MapVirtualKey('8', MAPVK_VK_TO_VSC)) };
    case '(': return { '9', true, static_cast<uint16_t>(MapVirtualKey('9', MAPVK_VK_TO_VSC)) };
    case ')': return { '0', true, static_cast<uint16_t>(MapVirtualKey('0', MAPVK_VK_TO_VSC)) };
    case '-': return { VK_OEM_MINUS, false, static_cast<uint16_t>(MapVirtualKey(VK_OEM_MINUS, MAPVK_VK_TO_VSC)) };
    case '_': return { VK_OEM_MINUS, true, static_cast<uint16_t>(MapVirtualKey(VK_OEM_MINUS, MAPVK_VK_TO_VSC)) };
    case '=': return { VK_OEM_PLUS, false, static_cast<uint16_t>(MapVirtualKey(VK_OEM_PLUS, MAPVK_VK_TO_VSC)) };
    case '+': return { VK_OEM_PLUS, true, static_cast<uint16_t>(MapVirtualKey(VK_OEM_PLUS, MAPVK_VK_TO_VSC)) };
    case '[': return { VK_OEM_4, false, static_cast<uint16_t>(MapVirtualKey(VK_OEM_4, MAPVK_VK_TO_VSC)) };
    case '{': return { VK_OEM_4, true, static_cast<uint16_t>(MapVirtualKey(VK_OEM_4, MAPVK_VK_TO_VSC)) };
    case ']': return { VK_OEM_6, false, static_cast<uint16_t>(MapVirtualKey(VK_OEM_6, MAPVK_VK_TO_VSC)) };
    case '}': return { VK_OEM_6, true, static_cast<uint16_t>(MapVirtualKey(VK_OEM_6, MAPVK_VK_TO_VSC)) };
    case ';': return { VK_OEM_1, false, static_cast<uint16_t>(MapVirtualKey(VK_OEM_1, MAPVK_VK_TO_VSC)) };
    case ':': return { VK_OEM_1, true, static_cast<uint16_t>(MapVirtualKey(VK_OEM_1, MAPVK_VK_TO_VSC)) };
    case '\'': return { VK_OEM_7, false, static_cast<uint16_t>(MapVirtualKey(VK_OEM_7, MAPVK_VK_TO_VSC)) };
    case '"': return { VK_OEM_7, true, static_cast<uint16_t>(MapVirtualKey(VK_OEM_7, MAPVK_VK_TO_VSC)) };
    case ',': return { VK_OEM_COMMA, false, static_cast<uint16_t>(MapVirtualKey(VK_OEM_COMMA, MAPVK_VK_TO_VSC)) };
    case '<': return { VK_OEM_COMMA, true, static_cast<uint16_t>(MapVirtualKey(VK_OEM_COMMA, MAPVK_VK_TO_VSC)) };
    case '.': return { VK_OEM_PERIOD, false, static_cast<uint16_t>(MapVirtualKey(VK_OEM_PERIOD, MAPVK_VK_TO_VSC)) };
    case '>': return { VK_OEM_PERIOD, true, static_cast<uint16_t>(MapVirtualKey(VK_OEM_PERIOD, MAPVK_VK_TO_VSC)) };
    case '/': return { VK_OEM_2, false, static_cast<uint16_t>(MapVirtualKey(VK_OEM_2, MAPVK_VK_TO_VSC)) };
    case '?': return { VK_OEM_2, true, static_cast<uint16_t>(MapVirtualKey(VK_OEM_2, MAPVK_VK_TO_VSC)) };
    case '\\': return { VK_OEM_5, false, static_cast<uint16_t>(MapVirtualKey(VK_OEM_5, MAPVK_VK_TO_VSC)) };
    case '|': return { VK_OEM_5, true, static_cast<uint16_t>(MapVirtualKey(VK_OEM_5, MAPVK_VK_TO_VSC)) };
    case '`': return { VK_OEM_3, false, static_cast<uint16_t>(MapVirtualKey(VK_OEM_3, MAPVK_VK_TO_VSC)) };
    case '~': return { VK_OEM_3, true, static_cast<uint16_t>(MapVirtualKey(VK_OEM_3, MAPVK_VK_TO_VSC)) };
    case ' ': return { VK_SPACE, false, static_cast<uint16_t>(MapVirtualKey(VK_SPACE, MAPVK_VK_TO_VSC)) };
    default:  return { 0, false, 0 };
    }
}

// 解析修饰键字符串
Send::KeyboardModifiers ParseModifiers(const std::vector<std::string>& parts) {
    Send::KeyboardModifiers mods;
    for (auto& p : parts) {
        if (p == "Ctrl") mods.LCtrl = true;
        else if (p == "Shift") mods.LShift = true;
        else if (p == "Alt") mods.LAlt = true;
        else if (p == "Win") mods.LWin = true;
    }
    return mods;
}

// 拆分字符串
std::vector<std::string> SplitString(const std::string& s, char delim) {
    std::vector<std::string> result;
    std::string tmp;
    for (char c : s) {
        if (c == delim) {
            if (!tmp.empty()) result.push_back(tmp);
            tmp.clear();
        }
        else tmp.push_back(c);
    }
    if (!tmp.empty()) result.push_back(tmp);
    return result;
}


// 清空系统和发送模块里的修饰键状态
void ReleaseAllModifiers() {
    // 系统层释放
    keybd_event(VK_SHIFT, 0, KEYEVENTF_KEYUP, 0);
    keybd_event(VK_CONTROL, 0, KEYEVENTF_KEYUP, 0);
    keybd_event(VK_MENU, 0, KEYEVENTF_KEYUP, 0);
    keybd_event(VK_LWIN, 0, KEYEVENTF_KEYUP, 0);
    keybd_event(VK_RWIN, 0, KEYEVENTF_KEYUP, 0);

    // 发送模块内部状态清零
    if (Send::g_send) {
        Send::g_send->sync_key_states();  // 保证内部状态和系统状态一致
    }
}

// 用户接口
DLLAPI bool SendHotkey(const std::string& hotkey) {
    // 先清空残留修饰键
    ReleaseAllModifiers();
    auto parts = SplitString(hotkey, '+');
    if (parts.empty()) return false;

    char targetChar = parts.back()[0];
    parts.pop_back();

    Send::KeyboardModifiers mods = ParseModifiers(parts);
    Send::CharMapping mapping = CharToVK(targetChar);

    if (mapping.shift) mods.LShift = true;

    return IbSendKeybdDownUp(mapping.vk, mapping.scancode, mods);
}
