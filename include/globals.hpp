#include "base.hpp"
#include <memory>

namespace main {
    // 全局发送对象，管理具体发送实现
    static std::unique_ptr<Send::Type::Internal::Base> send;
}
