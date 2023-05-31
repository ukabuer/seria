#pragma once
#include <mpack/mpack-node.h>
#include <seria/exception.hpp>
#include <seria/object.hpp>
#include <seria/type_traits.hpp>

namespace seria {

template <typename T>
std::enable_if_t<is_boolean<T>::value> deserialize(T &data,
                                                   const mpack_node_t &node) {
  auto value = mpack_node_bool(node);

  if (mpack_ok != mpack_node_error(node)) {
    throw type_error("boolean");
  }

  data = value;
}

template <typename T>
std::enable_if_t<is_integer<T>::value> deserialize(T &data,
                                                   const mpack_node_t &node) {
  auto value = mpack_node_int(node);

  if (mpack_ok != mpack_node_error(node)) {
    throw type_error("integer");
  }

  data = value;
}

template <typename T>
std::enable_if_t<is_unsigned_integer<T>::value>
deserialize(T &data, const mpack_node_t &node) {
  auto value = mpack_node_uint(node);

  if (mpack_ok != mpack_node_error(node)) {
    throw type_error("unsigned integer");
  }

  data = value;
}

template <typename T>
std::enable_if_t<is_float<T>::value> deserialize(T &data,
                                                 const mpack_node_t &node) {
  auto value = mpack_node_float(node);

  if (mpack_ok != mpack_node_error(node)) {
    throw type_error("float or double");
  }

  data = value;
}

template <typename T>
std::enable_if_t<std::is_enum<T>::value> deserialize(T &data,
                                                     const mpack_node_t &node) {
  auto value = mpack_node_int(node);

  if (mpack_ok != mpack_node_error(node)) {
    throw type_error("int");
  }

  data = static_cast<T>(value);
}

template <typename T>
std::enable_if_t<is_string<T>::value> deserialize(T &data,
                                                  const mpack_node_t &node) {
  auto value = mpack_node_str(node);

  if (mpack_ok != mpack_node_error(node)) {
    throw type_error("string");
  }

  auto len = mpack_node_strlen(node);
  data = std::string(value, len);
}

template <typename T>
std::enable_if_t<is_vector<T>::value> deserialize(T &data,
                                                  const mpack_node_t &node) {
  auto size = mpack_node_array_length(node);
  if (mpack_ok != mpack_node_error(node)) {
    throw type_error("array");
  }

  data.resize(size);
  for (size_t i = 0; i < size; i++) {
    try {
      auto child_node = mpack_node_array_at(node, i);
      deserialize(data[i], child_node);
    } catch (type_error &err) {
      err.add_prefix(std::to_string(i));
      throw err;
    } catch (error &err) {
      err.add_prefix(std::to_string(i));
      throw err;
    }
  }
}

template <typename T>
std::enable_if_t<is_array<T>::value> deserialize(T &data,
                                                 const mpack_node_t &node) {
  auto size = mpack_node_array_length(node);
  if (mpack_ok != mpack_node_error(node)) {
    throw type_error("array");
  }

  if (size != is_array<T>::size) {
    throw error("the size of array is not same with target");
  }

  for (size_t i = 0; i < size; i++) {
    try {
      auto child_node = mpack_node_array_at(node, i);
      deserialize(data[i], child_node);
    } catch (type_error &err) {
      err.add_prefix(std::to_string(i));
      throw err;
    } catch (error &err) {
      err.add_prefix(std::to_string(i));
      throw err;
    }
  }
}

template <typename T>
std::enable_if_t<is_object<T>::value> deserialize(T &data,
                                                  const mpack_node_t &node) {
  auto &members =
      KeyValueRecords<T, decltype(register_object<std::decay_t<T>>())>::members;

  constexpr size_t member_size =
      std::tuple_size<std::decay_t<decltype(members)>>::value;

  static_assert(member_size != 0, "No registered members!");

  if (node.data->type != mpack_type_map) {
    throw type_error("object");
  }

  auto setter = [&data, &node](auto &member) {
    if (!mpack_node_map_contains_cstr(node, member.m_key)) {
      if (member.m_default_value == nullptr) {
        throw error(member.m_key, "missing value");
      }
      
      member.set(data, *member.m_default_value);
      return;
    }

    try {
      auto value_node = mpack_node_map_cstr(node, member.m_key);
      typename std::remove_reference_t<decltype(member)>::Type tmp_val;
      deserialize(tmp_val, value_node);
      member.set(data, tmp_val);
    } catch (type_error &err) {
      err.add_prefix(member.m_key);
      throw err;
    } catch (error &err) {
      err.add_prefix(member.m_key);
      throw err;
    }
  };

  for_each(setter, members, std::make_index_sequence<member_size>());
}

} // namespace seria
