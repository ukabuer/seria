#pragma once
#include <mpack/mpack-writer.h>
#include <seria/object.hpp>
#include <seria/type_traits.hpp>
#include <string>

namespace seria {

template <typename T>
std::enable_if_t<std::is_arithmetic<T>::value>
serialize(const T &obj, mpack_writer_t *writer) {
  mpack_write(writer, obj);
}

template <typename T>
std::enable_if_t<std::is_enum<T>::value> serialize(const T &obj,
                                                   mpack_writer_t *writer) {
  mpack_write(writer, static_cast<int>(obj));
}

template <typename T>
std::enable_if_t<is_string<T>::value> serialize(const T &obj,
                                                mpack_writer_t *writer) {
  mpack_write(writer, obj);
}

template <typename T>
std::enable_if_t<is_object<T>::value> serialize(const T &obj,
                                                mpack_writer_t *writer) {
  auto &members = KeyValueRecords<T, decltype(register_object<T>())>::members;
  constexpr size_t member_size =
      std::tuple_size<std::decay_t<decltype(members)>>::value;

  static_assert(member_size != 0, "No registered members!");

  auto setter = [&obj, writer](auto &member) {
    auto &field = obj.*(member.m_ptr);
    mpack_write_str(writer, member.m_key, strlen(member.m_key));
    serialize(field, writer);
  };

  mpack_start_map(writer, member_size);
  for_each(setter, members, std::make_index_sequence<member_size>());
  mpack_finish_map(writer);
}

template <typename T>
std::enable_if_t<is_array<T>::value> serialize(const T &obj,
                                               mpack_writer_t *writer) {
  auto size = is_array<T>::size;
  mpack_start_array(writer, size);
  for (auto &value : obj) {
    serialize<std::decay_t<decltype(value)>>(value, writer);
  }
  mpack_finish_array(writer);
}

template <typename T>
std::enable_if_t<is_vector<T>::value> serialize(const T &obj,
                                                mpack_writer_t *writer) {
  mpack_start_array(writer, obj.size());
  for (auto &value : obj) {
    serialize<std::decay_t<decltype(value)>>(value, writer);
  }
  mpack_finish_array(writer);
}

} // namespace seria