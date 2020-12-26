#pragma once
#include <seria/object.hpp>
#include <seria/type_traits.hpp>
#ifdef SERIA_USE_EXTERNAL_RAPIDJSON
#include <rapidjson/document.h>
#else
#include <seria/rapidjson/document.h>
#endif

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

#include <seria/deserialize/rapidjson-inl.hpp>