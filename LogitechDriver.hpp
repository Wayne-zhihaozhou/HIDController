#pragma once
#include <string>
#include <Windows.h>

namespace Send::Type::Internal {

    /**
     * @brief Logitech 设备底层驱动
     *
     * 提供创建、销毁和键盘报告发送接口。
     */
    class LogitechDriver {
    public:
        /**
         * @brief 创建 Logitech 设备
         * @return Error 成功返回 Error::Success，找不到设备返回 Error::DeviceNotFound，打开失败返回 Error::DeviceOpenFailed
         */
        Error create();

        /**
         * @brief 销毁 Logitech 设备
         */
        void destroy();

        /**
         * @brief 发送键盘报告
         * @param report KeyboardReport 键盘报告
         * @return bool 发送成功返回 true
         */
        bool report_keyboard(KeyboardReport report) const;

    private:
        HANDLE device{ INVALID_HANDLE_VALUE };

        /**
         * @brief 查找 Logitech 设备
         * @return std::wstring 设备路径，未找到返回空字符串
         */
        std::wstring find_device();
    };

}  // namespace Send::Type::Internal
