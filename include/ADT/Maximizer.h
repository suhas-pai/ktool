//
//  ADT/Maximizer.h
//  ktool
//
//  Created by suhaspai on 11/22/22.
//

#include <concepts>

namespace ADT {
    template <std::integral T>
    struct Maximizer {
    protected:
        T Value;
    public:
        constexpr Maximizer() noexcept = default;
        constexpr Maximizer(T Value) noexcept : Value(Value) {}

        constexpr auto operator=(const T Value) noexcept -> decltype(*this) {
            if (this->Value < Value) {
                this->Value = Value;
            }

            return *this;
        }

        [[nodiscard]] constexpr auto value() const noexcept { return Value; }
    };
}
