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

template <typename Object, typename T> struct MemberBase {
  const char *m_key = "";
  std::unique_ptr<T> m_default_value;
  using Type = T;

  MemberBase(const char *key, std::unique_ptr<T> default_value)
      : m_key(key), m_default_value(std::move(default_value)) {}

  virtual T get(const Object &obj) = 0;
  virtual void set(Object &obj, const T &new_val) = 0;
};

template <typename Object, typename T, typename R, bool is_getter_setter = true,
          bool is_reference = true>
struct Member;

template <typename Object, typename T, typename R>
struct Member<Object, T, R, true, true> : public MemberBase<Object, T> {
  R (Object::*m_setter)(const T &) = nullptr;
  T (Object::*m_getter)() const = nullptr;

  using MemberBase<Object, T>::MemberBase;

  Member(const char *key, R (Object::*setter)(const T &),
         T (Object::*getter)() const, std::unique_ptr<T> default_value)
      : MemberBase<Object, T>(key, std::move(default_value)), m_setter(setter),
        m_getter(getter) {}

  virtual T get(const Object &obj) override { return (obj.*m_getter)(); };
  virtual void set(Object &obj, const T &new_val) override {
    (obj.*m_setter)(new_val);
  };
};

template <typename Object, typename T, typename R>
struct Member<Object, T, R, true, false> : public MemberBase<Object, T> {
  R (Object::*m_setter)(T) = nullptr;
  T (Object::*m_getter)() const = nullptr;

  using MemberBase<Object, T>::MemberBase;

  Member(const char *key, R (Object::*setter)(T), T (Object::*getter)() const,
         std::unique_ptr<T> default_value)
      : MemberBase<Object, T>(key, std::move(default_value)), m_setter(setter),
        m_getter(getter) {}

  virtual T get(const Object &obj) override { return (obj.*m_getter)(); };
  virtual void set(Object &obj, const T &new_val) override {
    (obj.*m_setter)(new_val);
  };
};

template <typename Object, typename T, typename R, bool is_reference>
struct Member<Object, T, R, false, is_reference>
    : public MemberBase<Object, T> {
  T Object::*m_ptr = nullptr;

  using MemberBase<Object, T>::MemberBase;
  
  Member(const char *key, T Object::*ptr, std::unique_ptr<T> default_value)
      : MemberBase<Object, T>(key, std::move(default_value)), m_ptr(ptr) {}
  
  virtual T get(const Object &obj) override { return obj.*m_ptr; };
  virtual void set(Object &obj, const T &new_val) override {
    obj.*m_ptr = new_val;
  };
};


template <typename Object, typename T>
constexpr auto member(const char *key, T Object::*ptr) {
  return Member<Object, T, void, false, false>{key, ptr, nullptr};
}

template <typename Object, typename T>
constexpr auto member(const char *key, T Object::*ptr, T &&default_value) {
  return Member<Object, T, void, false, false>{
      key, ptr, std::make_unique<T>(std::forward<T>(default_value))};
}

template <typename Object, typename T, typename R>
constexpr auto member(const char *key, R (Object::*setter)(const T &),
                      T (Object::*getter)() const) {
  return Member<Object, T, R>{key, setter, getter, nullptr};
}

template <typename Object, typename T, typename R>
constexpr auto member(const char *key, R (Object::*setter)(T),
                      T (Object::*getter)() const) {
  return Member<Object, T, R, true, false>{key, setter, getter, nullptr};
}

template <typename Object, typename T, typename R>
constexpr auto member(const char *key, R (Object::*setter)(T),
                      T (Object::*getter)() const, T &&default_value) {
  return Member<Object, T, R, true, false>{
      key, setter, getter, std::make_unique<T>(std::forward<T>(default_value))};
}

template <typename Object, typename T, typename R>
constexpr auto member(const char *key, R (Object::*setter)(const T &),
                      T (Object::*getter)() const, T &&default_value) {
  return Member<Object, T, R>{
      key, setter, getter, std::make_unique<T>(std::forward<T>(default_value))};
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