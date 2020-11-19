#pragma once
#include <array>
#include <string>
#include <type_traits>
#include <vector>

namespace seria {

template <typename T> struct is_integer : std::false_type {};
template <> struct is_integer<int8_t> : std::true_type {};
template <> struct is_integer<int16_t> : std::true_type {};
template <> struct is_integer<int32_t> : std::true_type {};

template <typename T> struct is_unsigned_integer : std::false_type {};
template <> struct is_unsigned_integer<uint8_t> : std::true_type {};
template <> struct is_unsigned_integer<uint16_t> : std::true_type {};
template <> struct is_unsigned_integer<uint32_t> : std::true_type {};

template <typename T> struct is_float : std::false_type {};
template <> struct is_float<float> : std::true_type {};
template <> struct is_float<double> : std::true_type {};

template <typename T> struct is_boolean : std::false_type {};
template <> struct is_boolean<bool> : std::true_type {};

template <typename T, typename _ = void> struct is_vector : std::false_type {};

template <typename T>
struct is_vector<T, std::enable_if_t<std::is_same<
                        T, std::vector<typename T::value_type,
                                       typename T::allocator_type>>::value>>
    : std::true_type {};

template <typename T> struct is_array : std::false_type {};

template <typename T, size_t N>
struct is_array<std::array<T, N>> : std::true_type {};

template <typename T, size_t N> struct is_array<T[N]> : std::true_type {};

template <typename T> struct is_string : std::false_type {};

template <> struct is_string<std::string> : std::true_type {};

template <typename T, typename _ = void> struct is_object : std::false_type {};

template <typename T>
struct is_object<
    T, std::enable_if_t<(!is_array<T>::value) && (!is_string<T>::value) &&
                        (!is_vector<T>::value && std::is_class<T>::value)>>
    : public std::true_type {};

} // namespace seria
