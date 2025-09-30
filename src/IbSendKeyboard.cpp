#include <bit>
#include <cassert>
#include <IbSendInput.hpp>
#include "IbSendKeyboard.hpp"

namespace Send {

	DLLAPI VOID WINAPI IbSend_keybd_event(
		_In_ BYTE bVk,
		_In_ BYTE bScan,
		_In_ DWORD dwFlags,
		_In_ ULONG_PTR dwExtraInfo
	) {
		INPUT input{
			.type = INPUT_KEYBOARD,
			.ki {
				.wVk = bVk,
				.wScan = bScan,
				.dwFlags = dwFlags,
				.time = 0,
				.dwExtraInfo = dwExtraInfo
			}
		};
		IbSendInput(1, &input, sizeof(INPUT));
	}

	DLLAPI bool __stdcall IbSendKeybdDown(uint16_t vk) {
		INPUT input{
			.type = INPUT_KEYBOARD,
			.ki {
				.wVk = vk,
				.wScan = 0,
				.dwFlags = 0,
				.time = 0,
				.dwExtraInfo = 0
			}
		};
		return IbSendInput(1, &input, sizeof(INPUT));
	}

	DLLAPI bool __stdcall IbSendKeybdUp(uint16_t vk) {
		INPUT input{
			.type = INPUT_KEYBOARD,
			.ki {
				.wVk = vk,
				.wScan = 0,
				.dwFlags = KEYEVENTF_KEYUP,
				.time = 0,
				.dwExtraInfo = 0
			}
		};
		return IbSendInput(1, &input, sizeof(INPUT));
	}

	DLLAPI bool __stdcall IbSendKeybdDownUp(uint16_t vk, Send::KeyboardModifiers modifiers) {
		constexpr INPUT blank_input = INPUT{ .type = INPUT_KEYBOARD, .ki { 0, 0, 0, 0, 0 } };

		INPUT inputs[18];  //720 bytes
		uint32_t i = 0;

#define CODE_GENERATE(vk, member)  \
    if (modifiers.member) {  \
        inputs[i] = blank_input;  \
        inputs[i].ki.wVk = vk;  \
        i++;  \
    }

		CODE_GENERATE(VK_LCONTROL, LCtrl)
			CODE_GENERATE(VK_RCONTROL, RCtrl)
			CODE_GENERATE(VK_LSHIFT, LShift)
			CODE_GENERATE(VK_RSHIFT, RShift)
			CODE_GENERATE(VK_LMENU, LAlt)
			CODE_GENERATE(VK_RMENU, RAlt)
			CODE_GENERATE(VK_LWIN, LWin)
			CODE_GENERATE(VK_RWIN, RWin)
#undef CODE_GENERATE
			inputs[i] = blank_input;
		inputs[i].ki.wVk = vk;
		i++;

		inputs[i] = inputs[i - 1];
		inputs[i].ki.dwFlags = KEYEVENTF_KEYUP;
		i++;
#define CODE_GENERATE(vk, member)  \
    if (modifiers.member) {  \
        inputs[i] = blank_input;  \
        inputs[i].ki.wVk = vk;  \
        inputs[i].ki.dwFlags = KEYEVENTF_KEYUP;  \
        i++;  \
    }

		CODE_GENERATE(VK_RWIN, RWin)
			CODE_GENERATE(VK_LWIN, LWin)
			CODE_GENERATE(VK_RMENU, RAlt)
			CODE_GENERATE(VK_LMENU, LAlt)
			CODE_GENERATE(VK_RSHIFT, RShift)
			CODE_GENERATE(VK_LSHIFT, LShift)
			CODE_GENERATE(VK_RCONTROL, RCtrl)
			CODE_GENERATE(VK_LCONTROL, LCtrl)
#undef CODE_GENERATE

			return IbSendInput(i, inputs, sizeof(INPUT)) == i;
	}

}