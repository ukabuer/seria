#pragma once
#include <array>
#include <functional>
#include <memory>
#include <string>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>

namespace seria {

template <typename Object, typename T> struct Member {
  const char *m_key = "";
  T Object::*m_ptr = nullptr;
  std::unique_ptr<T> m_default_value;
  using Type = T;
};

template <typename Object, typename T>
constexpr auto member(const char *key, T Object::*ptr,
                      std::unique_ptr<T> default_value = nullptr) {
  return Member<Object, T>{key, ptr, move(default_value)};
}

template <typename T, typename TupleType> struct KeyValueRecords {
  static TupleType members;
};

template <typename T> auto register_object() { return std::make_tuple(); }

template <typename T, typename TupleType>
TupleType KeyValueRecords<T, TupleType>::members = register_object<T>();

template <typename F, typename Tuple, size_t... I>
void for_each(F &&f, Tuple &&t, std::index_sequence<I...> /*unused*/) {
  using expand = std::initializer_list<bool>;
  (void)expand{(f(std::get<I>(std::forward<Tuple>(t))), true)...};
}

} // namespace seria