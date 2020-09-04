#pragma once
#include <rapidjson/document.h>
#include <seria/object.hpp>
#include <seria/type_traits.hpp>

namespace seria {

template <typename T>
std::enable_if_t<std::is_arithmetic<T>::value>
deserialize(T &data, const rapidjson::Value &value) {
  if (!value.IsBool() && !value.IsNumber()) {
    throw std::runtime_error("wrong type in JSON, should be arithmetic");
  }

  data = value.Get<T>();
}

template <typename T>
std::enable_if_t<is_string<T>::value>
deserialize(T &data, const rapidjson::Value &value) {
  if (!value.IsString()) {
    throw std::runtime_error("wrong type in JSON, should be string");
  }

  data = value.GetString();
}

template <typename T>
std::enable_if_t<is_vector<T>::value>
deserialize(T &data, const rapidjson::Value &value) {
  if (!value.IsArray()) {
    throw std::runtime_error("wrong type in JSON, should be array");
  }

  const auto size = value.Capacity();
  data.resize(size);
  for (size_t i = 0; i < size; i++) {
    deserialize(data[i], value[i]);
  }
}

template <typename T>
std::enable_if_t<is_array<T>::value>
deserialize(T &data, const rapidjson::Value &value) {
  if (!value.IsArray()) {
    throw std::runtime_error("wrong type in JSON, should be array");
  }

  const auto size = value.Capacity();
  if (size != data.size()) {
    throw std::runtime_error(
        "the size of array in JSON is not same with target");
  }

  for (size_t i = 0; i < size; i++) {
    deserialize(data[i], value[i]);
  }
}

template <typename Object, typename T>
void revert_helper(Object &data, const rapidjson::Value &value,
                   const Member<Object, T> &member) {
  deserialize(data.*(member.m_ptr), value);
}

template <typename Object, typename TargetType, typename InputType>
std::enable_if_t<std::is_arithmetic<InputType>::value> revert_helper(
    Object &data, const rapidjson::Value &value,
    const MemberWithTransform<Object, TargetType, InputType> &member) {
  if (!value.template Is<InputType>()) {
    throw std::runtime_error(std::string("wrong input type for ") +
                             member.m_key);
  }

  data.*(member.m_ptr) = member.m_revert(value.template Get<InputType>());
}

template <typename Object, typename TargetType, typename InputType>
std::enable_if_t<is_string<InputType>::value ||
                 std::is_same<InputType, const char *>::value>
revert_helper(
    Object &data, const rapidjson::Value &value,
    const MemberWithTransform<Object, TargetType, InputType> &member) {
  if (!value.IsString()) {
    throw std::runtime_error(std::string("wrong input type for ") +
                             member.m_key);
  }

  data.*(member.m_ptr) = member.m_revert(value.GetString());
}

template <typename Object, typename TargetType, typename InputType>
std::enable_if_t<!std::is_arithmetic<InputType>::value &&
                 !(is_string<InputType>::value ||
                   std::is_same<InputType, const char *>::value)>
revert_helper(Object &, const rapidjson::Value &,
              const MemberWithTransform<Object, TargetType, InputType> &) {
  static_assert(!std::is_same<Object, Object>::value,
                "object member with revert & transform function should have an"
                "input type of "
                "arithmetic or string");
}

template <typename T>
std::enable_if_t<is_object<T>::value>
deserialize(T &data, const rapidjson::Value &value) {
  auto &members =
      KeyValueRecords<T, decltype(register_object<std::decay_t<T>>())>::members;

  constexpr size_t member_size =
      std::tuple_size<std::decay_t<decltype(members)>>::value;

  static_assert(member_size != 0, "No registered members!");

  auto setter = [&data, &value](auto &member) {
    if (!value.HasMember(member.m_key)) {
      if (member.m_default_value == nullptr) {
        throw std::runtime_error(std::string("should have ") + member.m_key);
      }

      data.*(member.m_ptr) = *member.m_default_value;
      return;
    }

    revert_helper(data, value[member.m_key], member);
  };

  for_each(setter, members, std::make_index_sequence<member_size>());
}

} // namespace seria