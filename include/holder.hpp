#pragma once
#include <utility>
#include <cassert>

namespace ib {

    template<typename T>
    class Holder;

    template<>
    class Holder<void> {
    public:
        struct DefaultT {} static constexpr Default{};
    };

    template<typename T>
    class Holder {
        union {
            unsigned char buf[sizeof(T)];
            T v;
        };

    public:
#pragma warning(suppress : 26495)  // MEMBER_UNINIT
        constexpr Holder() {}
        ~Holder() {}

        constexpr Holder(Holder<void>::DefaultT) {
            new(buf) T();
        }

        template<typename... Ts>
        constexpr Holder(Ts&&... args) {
            new(buf) T(std::forward<Ts>(args)...);
        }

        template<typename... Ts>
        constexpr void create(Ts&&... args) {
            new(buf) T(std::forward<Ts>(args)...);
        }

        constexpr void destroy() {
            v.~T();
        }

        template<typename... Ts>
        constexpr void recreate(Ts&&... args) {
            destroy();
            new(buf) T(std::forward<Ts>(args)...);
        }

        constexpr T* operator->() {
            return &v;
        }

        constexpr const T* operator->() const {
            return &v;
        }

        constexpr T& operator*() {
            return v;
        }

        constexpr const T& operator*() const {
            return v;
        }
    };

    /// @brief Holder with a bool
    /// @warning Thread-unsafe
    template<typename T>
    class HolderB {
        union {
            unsigned char buf[sizeof(T)];
            T v;
        };
        bool b;

    public:
#pragma warning(suppress : 26495)  // MEMBER_UNINIT
        constexpr HolderB() : b(false) {}
        ~HolderB() {}

        constexpr HolderB(Holder<void>::DefaultT) {
            new(buf) T();
            b = true;
        }

        template<typename... Ts>
        constexpr HolderB(Ts&&... args) {
            new(buf) T(std::forward<Ts>(args)...);
            b = true;
        }

        constexpr bool created() const {
            return b;
        }

        template<typename... Ts>
        constexpr void create(Ts&&... args) {
            assert(!b);
            new(buf) T(std::forward<Ts>(args)...);
            b = true;
        }

        /// Do nothing if the object is not created
        constexpr void destroy() {
            if (b) {
                v.~T();
                b = false;
            }
        }

        /// Destroy only if the object is created
        template<typename... Ts>
        constexpr void recreate(Ts&&... args) {
            destroy();
            new(buf) T(std::forward<Ts>(args)...);
            b = true;
        }

        // for the convenience of accessing static members, don't assert(b)

        constexpr T* operator->() {
            return &v;
        }

        constexpr const T* operator->() const {
            return &v;
        }

        constexpr T& operator*() {
            return v;
        }

        constexpr const T& operator*() const {
            return v;
        }
    };
}