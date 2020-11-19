#pragma once
#include <rapidjson/document.h>
#include <seria/object.hpp>
#include <seria/type_traits.hpp>

namespace seria {

template <typename T>
std::enable_if_t<std::is_arithmetic<T>::value, rapidjson::Document>
serialize(const T &obj);

template <typename T>
std::enable_if_t<std::is_enum<T>::value, rapidjson::Document>
serialize(const T &obj);

template <typename T>
std::enable_if_t<is_string<T>::value, rapidjson::Document>
serialize(const T &obj);

template <typename T>
std::enable_if_t<is_object<T>::value, rapidjson::Document>
serialize(const T &obj);

template <typename T>
std::enable_if_t<is_vector<T>::value || is_array<T>::value, rapidjson::Document>
serialize(const T &obj);

template <typename T> std::string to_string(const T &obj);

} // namespace seria

#include "serialize.inl"