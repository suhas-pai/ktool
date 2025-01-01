/*
 * Utils/Templates.h
 * ktool
 *
 * Created by suhaspai on 12/31/24.
 */

#pragma once
#include <type_traits>

namespace Utils {
    template <typename... Ts>
    struct largest_type;

    template <typename T>
    struct largest_type<T> {
        using type = T;
    };

    template <typename T, typename U, typename... Ts>
    struct largest_type<T, U, Ts...> {
        using type =
            typename largest_type<typename std::conditional<
                (sizeof(U) <= sizeof(T)), T, U>::type, Ts...>::type;
    };
}
