#include <cstdint>
#include <type_traits>

namespace fw {
    class error {
    public:
        
        // constructor
        constexpr error() noexcept = default;

        /*
        constexpr error() noexcept
            : code_(static_cast<uint32_t>(fw::net::ErrorCode::Success)) {}
        */

        template <typename EnumType>
            requires std::is_enum_v<EnumType>
        constexpr error(EnumType code) noexcept
            : code_(static_cast<uint32_t>(code)) {}

        constexpr explicit error(uint32_t code) noexcept
            : code_(code) {}

        // assignment Operators
        template <typename EnumType>
            requires std::is_enum_v<EnumType>
        constexpr error& operator=(EnumType code) noexcept {
            code_ = static_cast<uint32_t>(code);
            return *this;
        }

        template <std::integral T>
        constexpr error& operator=(T code) noexcept {
            code_ = static_cast<uint32_t>(code);
            return *this;
        }

        // assign
        template <typename EnumType>
            requires std::is_enum_v<EnumType>
        constexpr void assign(EnumType code) noexcept {
            code_ = static_cast<uint32_t>(code);
        }

        constexpr void assign(uint32_t code) noexcept {
            code_ = code;
        }

        // implicit type conversion
        // std::integral 컨셉을 활용해 DWORD(unsigned long), int, uint32_t 등 모든 정수 타입으로 암시적 변환
        template <std::integral T>
        [[nodiscard]] constexpr operator T() const noexcept {
            return static_cast<T>(code_);
        }

        // bool check
        [[nodiscard]] constexpr bool fail() const noexcept {
            return code_ != 0;
        }

        [[nodiscard]] constexpr explicit operator bool() const noexcept {
            return fail();
        }

        [[nodiscard]] constexpr bool operator!() const noexcept {
            return !fail();
        }

        // Getters
        [[nodiscard]] constexpr uint32_t value() const noexcept { return code_; }

        /*
        template <typename EnumType = fw::net::ErrorCode>
        [[nodiscard]] constexpr EnumType code() const noexcept {
            return static_cast<EnumType>(code_);
        }
        */

        // Comparison Operators

        constexpr bool operator==(const error& rhs) const noexcept {
            return code_ == rhs.code_;
        }

        constexpr bool operator==(uint32_t rhs) const noexcept {
            return code_ == rhs;
        }

        template <typename EnumType>
            requires std::is_enum_v<EnumType>
        constexpr bool operator==(EnumType rhs) const noexcept {
            return code_ == static_cast<uint32_t>(rhs);
        }

    private:
        uint32_t code_{ 0 };
    };

    // 반대 방향 비교 (uint32_t == error 또는 Enum == error)
    constexpr bool operator==(uint32_t lhs, const error& rhs) noexcept {
        return rhs == lhs;
    }

    template <typename EnumType>
        requires std::is_enum_v<EnumType>
    constexpr bool operator==(EnumType lhs, const error& rhs) noexcept {
        return rhs == lhs;
    }

}