#pragma once
#include <rapidjson/document.h>
#include <seria/object.hpp>
#include <seria/type_traits.hpp>

namespace seria {

template <typename T>
std::enable_if_t<is_boolean<T>::value>
deserialize(T &data, const rapidjson::Value &value) {
  if (!value.IsBool()) {
    throw std::runtime_error("wrong type in JSON, should be boolean");
  }

  data = value.GetBool();
}

template <typename T>
std::enable_if_t<is_integer<T>::value>
deserialize(T &data, const rapidjson::Value &value) {
  if (!value.IsInt()) {
    throw std::runtime_error("wrong type in JSON, should be integer");
  }

  data = value.GetInt();
}

template <typename T>
std::enable_if_t<is_unsigned_integer<T>::value>
deserialize(T &data, const rapidjson::Value &value) {
  if (!value.IsUint()) {
    throw std::runtime_error("wrong type in JSON, should be unsigned integer");
  }

  data = value.GetUint();
}

template <typename T>
std::enable_if_t<is_float<T>::value>
deserialize(T &data, const rapidjson::Value &value) {
  if (!value.Is<T>()) {
    throw std::runtime_error("wrong type in JSON, should be float or double");
  }

  data = value.Get<T>();
}

template <typename T>
std::enable_if_t<std::is_enum<T>::value>
deserialize(T &data, const rapidjson::Value &value) {
  if (!value.IsInt()) {
    throw std::runtime_error("wrong type in JSON, should be int");
  }

  data = static_cast<T>(value.GetInt());
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

    deserialize(data.*(member.m_ptr), value[member.m_key]);
  };

  for_each(setter, members, std::make_index_sequence<member_size>());
}

} // namespace seria