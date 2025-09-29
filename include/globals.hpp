// globals.hpp
#pragma once
#include <memory>
#include "base.hpp"

namespace main {
    extern std::unique_ptr<Send::Type::Internal::Base> send;
}
