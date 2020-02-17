#pragma once
#include <rapidjson/document.h>
#include <seria/utils.hpp>

namespace seria {

template <typename T>
std::enable_if_t<std::is_arithmetic<T>::value>
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