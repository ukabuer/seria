#pragma once
#include <array>
#include <memory>
#include <string>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>

namespace seria {

template <typename Object, typename T> struct Member {
  const char *key;
  T Object::*ptr;
  std::unique_ptr<T> default_value;
  using Type = T;
};

template <typename Object, typename T>
constexpr auto member(const char *key, T Object::*ptr,
                      std::unique_ptr<T> default_value = nullptr) {
  return Member<Object, T>{key, ptr, move(default_value)};
}

template <typename T> auto registerObject() { return std::make_tuple(); }

template <typename T, typename TupleType> struct KeyValueRecords {
  static TupleType members;
};

template <typename T, typename TupleType>
TupleType KeyValueRecords<T, TupleType>::members = registerObject<T>();

template <typename F, typename... Args>
void for_each_arg(F &&f, Args &&... args) {
  using expand = bool[];
  (void)expand{(f(std::forward<Args>(args)), true)...};
}

template <typename F, typename Tuple, size_t... I>
decltype(auto) apply(F &&f, Tuple &&t, std::index_sequence<I...>) {
  return f(std::get<I>(std::forward<Tuple>(t))...);
}

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

template <typename T, typename _ = void> struct is_string : std::false_type {};

template <> struct is_string<std::string> : std::true_type {};

template <typename T, typename _ = void> struct is_object : std::false_type {};

template <typename T>
struct is_object<
    T, std::enable_if_t<(!is_array<T>::value) && (!is_string<T>::value) &&
                        (!is_vector<T>::value && std::is_class<T>::value)>>
    : public std::true_type {};

} // namespace seria