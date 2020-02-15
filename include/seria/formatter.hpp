#pragma once
#include <iostream>
#include <memory>
#include <rapidjson/document.h>
#include <seria/utils.hpp>
#include <string>
#include <tuple>
#include <type_traits>
#include <unordered_map>
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

template <typename T>
auto serialize(const T &obj,
               std::enable_if_t<std::is_arithmetic<T>::value> * = nullptr)
    -> rapidjson::Document;

template <typename T>
auto serialize(const T &obj, std::enable_if_t<is_object<T>::value> * = nullptr)
    -> rapidjson::Document;

template <typename T>
auto serialize(const T &obj,
               std::enable_if_t<is_vector<T>::value || is_array<T>::value> * =
                   nullptr) -> rapidjson::Document;

template <typename T>
auto serialize(const T &obj, std::enable_if_t<is_string<T>::value> * = nullptr)
    -> rapidjson::Document;

template <typename T>
void deserialize(T &data, const rapidjson::Value &value,
                 std::enable_if_t<std::is_arithmetic<T>::value> * = nullptr);

template <typename T>
void deserialize(T &data, const rapidjson::Value &value,
                 std::enable_if_t<is_vector<T>::value> * = nullptr);

template <typename T>
void deserialize(T &data, const rapidjson::Value &value,
                 std::enable_if_t<is_array<T>::value> * = nullptr);

template <typename T>
void deserialize(T &data, const rapidjson::Value &value,
                 std::enable_if_t<is_string<T>::value> * = nullptr);

template <typename T>
void deserialize(T &data, const rapidjson::Value &value,
                 std::enable_if_t<is_object<T>::value> * = nullptr);

template <typename T>
auto serialize(const T &obj, std::enable_if_t<std::is_arithmetic<T>::value> *)
    -> rapidjson::Document {
  rapidjson::Document document{};
  document.Set(obj, document.GetAllocator());

  return document;
}

template <typename T>
auto serialize(const T &obj,
               std::enable_if_t<is_vector<T>::value || is_array<T>::value> *)
    -> rapidjson::Document {
  rapidjson::Document json(rapidjson::kArrayType);
  auto &allocator = json.GetAllocator();

  for (auto &value : obj) {
    auto json_value = serialize<std::decay_t<decltype(value)>>(value);
    rapidjson::Value item(json_value, allocator);
    json.PushBack(item, allocator);
  }

  return json;
}

template <typename T>
auto serialize(const T &obj, std::enable_if_t<is_string<T>::value> *)
    -> rapidjson::Document {
  rapidjson::Document json(rapidjson::kStringType);
  auto &allocator = json.GetAllocator();

  json.SetString(obj.c_str(), obj.size(), allocator);

  return json;
}

template <typename T>
auto serialize(const T &obj, std::enable_if_t<is_object<T>::value> *)
    -> rapidjson::Document {
  rapidjson::Document document(rapidjson::kObjectType);
  auto &allocator = document.GetAllocator();

  auto &members = KeyValueRecords<T, decltype(registerObject<T>())>::members;

  constexpr size_t member_size =
      std::tuple_size<std::decay_t<decltype(members)>>::value;

  auto setter = [&obj, &document, &allocator](auto &member) {
    auto &field = obj.*(member.ptr);
    rapidjson::Value key(member.key, allocator);
    rapidjson::Value value(serialize(field).Move(), allocator);
    document.AddMember(key, value, allocator);
  };

  apply(
      [&setter](auto &&... args) {
        for_each_arg(setter, std::forward<decltype(args)>(args)...);
      },
      members, std::make_index_sequence<member_size>());

  return document;
}

template <typename T>
void deserialize(T &data, const rapidjson::Value &value,
                 std::enable_if_t<std::is_arithmetic<T>::value> *) {
  if (!value.IsBool() && !value.IsNumber()) {
    throw std::runtime_error("wrong type");
  }

  data = value.Get<T>();
}

template <typename T>
void deserialize(T &data, const rapidjson::Value &value,
                 std::enable_if_t<is_string<T>::value> *) {
  if (!value.IsString()) {
    throw std::runtime_error("wrong type");
  }

  data = value.GetString();
}

template <typename T>
void deserialize(T &data, const rapidjson::Value &value,
                 std::enable_if_t<is_vector<T>::value> *) {
  const auto size = value.Capacity();
  data.resize(size);
  for (size_t i = 0; i < size; i++) {
    deserialize(data[i], value[i]);
  }
}

template <typename T>
void deserialize(T &data, const rapidjson::Value &value,
                 std::enable_if_t<is_array<T>::value> *) {
  const auto size = value.Capacity();
  for (size_t i = 0; i < size; i++) {
    deserialize(data[i], value[i]);
  }
}

template <typename T>
void deserialize(T &data, const rapidjson::Value &value,
                 std::enable_if_t<is_object<T>::value> *) {
  auto &members =
      KeyValueRecords<T, decltype(registerObject<std::decay_t<T>>())>::members;

  constexpr size_t member_size =
      std::tuple_size<std::decay_t<decltype(members)>>::value;

  auto setter = [&data, &value](auto &member) {
    if (!value.HasMember(member.key)) {
      if (member.default_value == nullptr) {
        throw std::runtime_error(std::string("should have ") + member.key);
      }

      data.*(member.ptr) = *member.default_value;
      return;
    }

    deserialize(data.*(member.ptr), value[member.key]);
  };

  apply(
      [&setter](auto &&... args) {
        for_each_arg(setter, std::forward<decltype(args)>(args)...);
      },
      members, std::make_index_sequence<member_size>());
}

} // namespace seria