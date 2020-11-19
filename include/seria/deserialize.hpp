#pragma once
#include <rapidjson/document.h>
#include <seria/object.hpp>
#include <seria/type_traits.hpp>

namespace seria {

template <typename T>
std::enable_if_t<is_boolean<T>::value>
deserialize(T &data, const rapidjson::Value &value);

template <typename T>
std::enable_if_t<is_integer<T>::value>
deserialize(T &data, const rapidjson::Value &value);

template <typename T>
std::enable_if_t<is_unsigned_integer<T>::value>
deserialize(T &data, const rapidjson::Value &value);

template <typename T>
std::enable_if_t<is_float<T>::value> deserialize(T &data,
                                                 const rapidjson::Value &value);

template <typename T>
std::enable_if_t<std::is_enum<T>::value>
deserialize(T &data, const rapidjson::Value &value);

template <typename T>
std::enable_if_t<is_string<T>::value>
deserialize(T &data, const rapidjson::Value &value);

template <typename T>
std::enable_if_t<is_vector<T>::value>
deserialize(T &data, const rapidjson::Value &value);

template <typename T>
std::enable_if_t<is_array<T>::value> deserialize(T &data,
                                                 const rapidjson::Value &value);

template <typename T>
std::enable_if_t<is_object<T>::value>
deserialize(T &data, const rapidjson::Value &value);

} // namespace seria

#include "deserialize.inl"