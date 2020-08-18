#pragma once
#include <rapidjson/document.h>
#include <seria/object.hpp>
#include <seria/type_traits.hpp>

namespace seria {

template <typename T>
std::enable_if_t<std::is_arithmetic<T>::value, rapidjson::Document>
serialize(const T &obj) {
  rapidjson::Document document{};
  document.Set(obj, document.GetAllocator());

  return document;
}

template <typename T>
std::enable_if_t<is_string<T>::value, rapidjson::Document>
serialize(const T &obj) {
  rapidjson::Document json(rapidjson::kStringType);
  auto &allocator = json.GetAllocator();

  json.SetString(obj.c_str(), obj.size(), allocator);

  return json;
}

template <typename T>
std::enable_if_t<is_vector<T>::value || is_array<T>::value, rapidjson::Document>
serialize(const T &obj) {
  rapidjson::Document json(rapidjson::kArrayType);
  auto &allocator = json.GetAllocator();

  for (auto &value : obj) {
    auto json_value = serialize<std::decay_t<decltype(value)>>(value);
    rapidjson::Value item(json_value, allocator);
    json.PushBack(item, allocator);
  }

  return json;
}

template <typename Object, typename T>
rapidjson::Value transform_helper(const Object &data,
                                  rapidjson::Document &document,
                                  const Member<Object, T> &member) {
  auto &field = data.*(member.m_ptr);
  rapidjson::Value value(serialize(field).Move(), document.GetAllocator());
  return value;
}

template <typename Object, typename TargetType, typename InputType>
rapidjson::Value transform_helper(
    const Object &data, rapidjson::Document &document,
    const MemberWithTransform<Object, TargetType, InputType> &member) {
  auto &field = data.*(member.m_ptr);

  rapidjson::Document tmp{};
  tmp.Set(member.m_transform(field), document.GetAllocator());
  rapidjson::Value value(tmp, document.GetAllocator());
  return value;
}

template <typename T>
std::enable_if_t<is_object<T>::value, rapidjson::Document>
serialize(const T &obj) {
  rapidjson::Document document(rapidjson::kObjectType);
  auto &allocator = document.GetAllocator();

  auto &members = KeyValueRecords<T, decltype(register_object<T>())>::members;

  constexpr size_t member_size =
      std::tuple_size<std::decay_t<decltype(members)>>::value;

  static_assert(member_size != 0, "No registered members!");

  auto setter = [&obj, &document, &allocator](auto &member) {
    rapidjson::Value key(member.m_key, allocator);
    auto value = transform_helper(obj, document, member);
    document.AddMember(key, value, allocator);
  };

  for_each(setter, members, std::make_index_sequence<member_size>());

  return document;
}

} // namespace seria