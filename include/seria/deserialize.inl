#pragma once

namespace seria {

template <typename T>
std::enable_if_t<std::is_arithmetic<T>::value>
deserialize(T &data, const rapidjson::Value &value) {
  if (!value.IsBool() && !value.IsNumber()) {
    throw std::runtime_error("wrong type");
  }

  data = value.Get<T>();
}

template <typename T>
std::enable_if_t<is_string<T>::value>
deserialize(T &data, const rapidjson::Value &value) {
  if (!value.IsString()) {
    throw std::runtime_error("wrong type");
  }

  data = value.GetString();
}

template <typename T>
std::enable_if_t<is_vector<T>::value>
deserialize(T &data, const rapidjson::Value &value) {
  const auto size = value.Capacity();
  data.resize(size);
  for (size_t i = 0; i < size; i++) {
    deserialize(data[i], value[i]);
  }
}

template <typename T>
std::enable_if_t<is_array<T>::value>
deserialize(T &data, const rapidjson::Value &value) {
  const auto size = value.Capacity();
  for (size_t i = 0; i < size; i++) {
    deserialize(data[i], value[i]);
  }
}

template <typename T>
std::enable_if_t<is_object<T>::value>
deserialize(T &data, const rapidjson::Value &value) {
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