#pragma once
#include <rapidjson/document.h>
#include <seria/exception.hpp>
#include <seria/object.hpp>
#include <seria/type_traits.hpp>

namespace seria {

template <typename T>
std::enable_if_t<is_boolean<T>::value>
deserialize(T &data, const rapidjson::Value &value) {
  if (!value.IsBool()) {
    throw type_error("boolean");
  }

  data = value.GetBool();
}

template <typename T>
std::enable_if_t<is_integer<T>::value>
deserialize(T &data, const rapidjson::Value &value) {
  if (!value.IsInt()) {
    throw type_error("integer");
  }

  data = value.GetInt();
}

template <typename T>
std::enable_if_t<is_unsigned_integer<T>::value>
deserialize(T &data, const rapidjson::Value &value) {
  if (!value.IsUint()) {
    throw type_error("unsigned integer");
  }

  data = value.GetUint();
}

template <typename T>
std::enable_if_t<is_float<T>::value>
deserialize(T &data, const rapidjson::Value &value) {
  if (!value.Is<T>() && !value.IsInt()) {
    throw type_error("float or double");
  }

  if (value.Is<T>()) {
    data = value.Get<T>();
  } else {
    data = static_cast<T>(value.GetInt());
  }
}

template <typename T>
std::enable_if_t<std::is_enum<T>::value>
deserialize(T &data, const rapidjson::Value &value) {
  if (!value.IsInt()) {
    throw type_error("int");
  }

  data = static_cast<T>(value.GetInt());
}

template <typename T>
std::enable_if_t<is_string<T>::value>
deserialize(T &data, const rapidjson::Value &value) {
  if (!value.IsString()) {
    throw type_error("string");
  }

  data = value.GetString();
}

template <typename T>
std::enable_if_t<is_vector<T>::value>
deserialize(T &data, const rapidjson::Value &value) {
  if (!value.IsArray()) {
    throw type_error("array");
  }

  const auto size = value.Capacity();
  data.resize(size);
  for (size_t i = 0; i < size; i++) {
    try {
      deserialize(data[i], value[i]);
    } catch (type_error &err) {
      err.add_prefix(std::to_string(i));
      throw err;
    } catch (error &err) {
      err.add_prefix(std::to_string(i));
      throw err;
    }
  }
}

template <typename T>
std::enable_if_t<is_array<T>::value>
deserialize(T &data, const rapidjson::Value &value) {
  if (!value.IsArray()) {
    throw type_error("array");
  }

  const auto size = value.Capacity();
  if (size != is_array<T>::size) {
    throw error("the size of array is not same with target");
  }

  for (size_t i = 0; i < size; i++) {
    try {
      deserialize(data[i], value[i]);
    } catch (type_error &err) {
      err.add_prefix(std::to_string(i));
      throw err;
    } catch (error &err) {
      err.add_prefix(std::to_string(i));
      throw err;
    }
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

  if (!value.IsObject()) {
    throw type_error("object");
  }

  auto setter = [&data, &value](auto &member) {
    if (!value.HasMember(member.m_key)) {
      if (member.m_default_value == nullptr) {
        throw error(member.m_key, "missing value");
      }

      data.*(member.m_ptr) = *member.m_default_value;
      return;
    }

    try {
      deserialize(data.*(member.m_ptr), value[member.m_key]);
    } catch (type_error &err) {
      err.add_prefix(member.m_key);
      throw err;
    } catch (error &err) {
      err.add_prefix(member.m_key);
      throw err;
    }
  };

  for_each(setter, members, std::make_index_sequence<member_size>());
}

} // namespace seria