///
// utility.hpp
//
// Stores common utility functions used by internally.

#pragma once

#include <type_traits>

namespace mpe {

// A general point type. This may be typedef'd in a number of places for
// explicitness.
struct point {
    int x;
    int y;
};

// Cast an enum class value to its underlying type (usually int).
template <typename T>
constexpr typename std::underlying_type<T>::type enum_cast(T t)
{
    return static_cast<typename std::underlying_type<T>::type>(t);
}

} // namespace mpe
