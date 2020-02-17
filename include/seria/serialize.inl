#pragma once

namespace seria {

template <typename T>
std::enable_if_t<std::is_arithmetic<T>::value, rapidjson::Document>
serialize(const T &obj) {
  rapidjson::Document document{};
  document.Set(obj, document.GetAllocator());

  return document;
}

template <typename T>
std::enable_if_t<is_string<T>::value> serialize(const T &obj) {
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

template <typename T>
std::enable_if_t<is_object<T>::value, rapidjson::Document>
serialize(const T &obj) {
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

} // namespace seria