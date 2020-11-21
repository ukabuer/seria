#pragma once
#include <stdexcept>
#include <utility>

namespace seria {

class error : public std::exception {
public:
  explicit error(std::string msg) : error("", std::move(msg)) {}

  error(std::string path, std::string msg)
      : m_path(std::move(path)), m_msg(std::move(msg)),
        m_err(m_path.empty() ? m_msg : (m_path + ": " + m_msg)) {}

  void add_prefix(const std::string &prefix) {
    if (prefix.empty()) {
      return;
    }

    if (m_path.empty()) {
      m_path = prefix;
    } else {
      m_path = prefix + "." + m_path;
    }

    m_err = m_path + ": " + m_msg;
  }

  const char *what() const noexcept override { return m_err.c_str(); }

  const char *path() const noexcept { return m_path.c_str(); }

private:
  std::string m_path;
  std::string m_msg;
  std::string m_err;
};

class type_error : public error {
public:
  explicit type_error(std::string desired_type)
      : error("wrong type, should be " + desired_type),
        m_desired_type(std::move(desired_type)) {}

  type_error(std::string path, std::string desired_type)
      : error(std::move(path), "wrong type, should be " + desired_type),
        m_desired_type(std::move(desired_type)) {}

  const char *desired_type() const noexcept { return m_desired_type.c_str(); }

private:
  std::string m_desired_type;
};

} // namespace seria