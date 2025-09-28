#pragma once
#include <cstdint>
#include <cstddef>

namespace ib {
    using int8 = std::int8_t;
    using int16 = std::int16_t;
    using int32 = std::int32_t;
    using int64 = std::int64_t;
    using uint8 = std::uint8_t;
    using uint16 = std::uint16_t;
    using uint32 = std::uint32_t;
    using uint64 = std::uint64_t;

    using int_least8 = std::int_least8_t;
    using int_least16 = std::int_least16_t;
    using int_least32 = std::int_least32_t;
    using int_least64 = std::int_least64_t;
    using uint_least8 = std::uint_least8_t;
    using uint_least16 = std::uint_least16_t;
    using uint_least32 = std::uint_least32_t;
    using uint_least64 = std::uint_least64_t;

    using int_fast8 = std::int_fast8_t;
    using int_fast16 = std::int_fast16_t;
    using int_fast32 = std::int_fast32_t;
    using int_fast64 = std::int_fast64_t;
    using uint_fast8 = std::uint_fast8_t;
    using uint_fast16 = std::uint_fast16_t;
    using uint_fast32 = std::uint_fast32_t;
    using uint_fast64 = std::uint_fast64_t;

    using intmax = std::intmax_t;
    using intptr = std::intptr_t;
    using uintmax = std::uintmax_t;
    using uintptr = std::uintptr_t;

    using wchar = wchar_t;
#ifdef __cpp_char8_t
    using char8 = char8_t;
#endif
    using char16 = char16_t;
    using char32 = char32_t;

    using ptrdiff = std::ptrdiff_t;
    using size = std::size_t;
    using max_align = std::max_align_t;
    
    using Byte = uint8;
}