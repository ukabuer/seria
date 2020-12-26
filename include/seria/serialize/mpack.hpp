#pragma once
#include <mpack/mpack-writer.h>
#include <seria/object.hpp>
#include <seria/type_traits.hpp>

namespace seria {

template <typename T>
std::enable_if_t<std::is_arithmetic<T>::value>
serialize(const T &obj, mpack_writer_t *writer);

template <typename T>
std::enable_if_t<std::is_enum<T>::value> serialize(const T &obj,
                                                   mpack_writer_t *writer);

template <typename T>
std::enable_if_t<is_string<T>::value> serialize(const T &obj,
                                                mpack_writer_t *writer);

template <typename T>
std::enable_if_t<is_object<T>::value> serialize(const T &obj,
                                                mpack_writer_t *writer);

template <typename T>
std::enable_if_t<is_array<T>::value> serialize(const T &obj,
                                               mpack_writer_t *writer);

template <typename T>
std::enable_if_t<is_vector<T>::value &&
                 std::is_same<typename T::value_type, uint8_t>::value>
serialize(const T &obj, mpack_writer_t *writer);

template <typename T>
std::enable_if_t<is_vector<T>::value &&
                 !std::is_same<typename T::value_type, uint8_t>::value>
serialize(const T &obj, mpack_writer_t *writer);

} // namespace seria

#include <seria/serialize/mpack-inl.hpp>