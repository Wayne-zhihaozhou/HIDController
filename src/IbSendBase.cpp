#include <IbSendBase.hpp>
#include <Logitech.hpp>
#include <IbSendInput.hpp>

// 初始化发送模块（仅支持 Logitech），创建并保存全局发送对象
DLLAPI Send::Error __stdcall IbSendInit(Send::SendType type, Send::InitFlags flags, void* argument) {
    if (type != Send::SendType::Logitech) {
        return Send::Error::InvalidArgument;  // 只支持 Logitech
    }

    auto logitech = std::make_unique<Send::Type::Internal::Logitech>();
    logitech->create_base(&SendInputHook::GetAsyncKeyState_real);
    Send::Error error = logitech->create();
    if (error != Send::Error::Success)
        return error;

    Send::g_send = std::move(logitech); 
    return Send::Error::Success;
}


// 销毁发送模块，释放全局发送对象
DLLAPI void __stdcall IbSendDestroy() {
    IbSendInputHook(Send::HookCode::Destroy);

    if (!Send::g_send)
        return;
    Send::g_send->destroy();
    Send::g_send.reset();
}

// 同步按键状态到发送模块
DLLAPI void __stdcall IbSendSyncKeyStates() {
    Send::g_send->sync_key_states(); 
}