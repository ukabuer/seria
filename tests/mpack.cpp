#include <catch2/catch_all.hpp>
#include <iostream>
#include <seria/deserialize/mpack.hpp>
#include <seria/serialize/mpack.hpp>

using namespace std;

struct Inside {
  int i_age = 1;
  float i_value = 1.0f;
  std::vector<int> i_v = {1, 2, 3, 4, 5};
};

enum class Gender { Male = 0, Female = 1 };

struct Person {
  int age = 1;
  float value = 1.0f;
  Gender gender = Gender::Male;
  uint32_t test_uint = 1;
  Inside inside{};
};

enum class Child { Boy, Girl };

namespace seria {

template <> auto register_object<Person>() {
  return std::make_tuple(member("age", &Person::age, 50),
                         member("value", &Person::value),
                         member("gender", &Person::gender, Gender::Male),
                         member("test_uint", &Person::test_uint),
                         member("inside", &Person::inside));
}

template <> auto register_object<Inside>() {
  return std::make_tuple(member("i_age", &Inside::i_age, 100),
                         member("i_value", &Inside::i_value),
                         member("i_v", &Inside::i_v));
}

template <> void serialize(const Child &data, mpack_writer_t *writer) {
  if (data == Child::Boy) {
    mpack_write_str(writer, "B", 1);
  } else {
    mpack_write_str(writer, "G", 1);
  }
}

template <> void deserialize(Child &data, const mpack_node_t &node) {
  if (node.data->type != mpack_type_str) {
    throw type_error("", "should be string");
  }

  auto len = mpack_node_strlen(node);
  if (node.data->type != mpack_type_str) {
    throw type_error("", "should be `B` or `G`");
  }

  if (std::strncmp("B", mpack_node_str(node), 1) == 0) {
    data = Child::Boy;
  } else {
    data = Child::Girl;
  }
}

} // namespace seria

namespace seria {} // namespace seria

TEST_CASE("serialize c style array", "[serialize]") {
  int a[] = {1, 2, 3, 4, 5};

  char *data = nullptr;
  size_t total = 0;
  mpack_writer_t writer;
  mpack_writer_init_growable(&writer, &data, &total);
  seria::serialize(a, &writer);
  auto result = mpack_writer_destroy(&writer);

  uint8_t target[] = {0x95, 0x1, 0x2, 0x3, 0x4, 0x5};
  REQUIRE(result == mpack_ok);
  REQUIRE(std::memcmp(data, target, total) == 0);

  free(data);
}

TEST_CASE("serialize std::array", "[serialize]") {
  array<int, 5> a = {1, 2, 3, 4, 5};

  char *data = nullptr;
  size_t total = 0;
  mpack_writer_t writer;
  mpack_writer_init_growable(&writer, &data, &total);
  seria::serialize(a, &writer);
  auto result = mpack_writer_destroy(&writer);

  uint8_t target[] = {0x95, 0x1, 0x2, 0x3, 0x4, 0x5};
  REQUIRE(result == mpack_ok);
  REQUIRE(std::memcmp(data, target, total) == 0);

  free(data);
}

TEST_CASE("serialize std::vector", "[serialize]") {
  vector<int> a = {1, 2, 3, 4};
  a.push_back(5);

  char *data = nullptr;
  size_t total = 0;
  mpack_writer_t writer;
  mpack_writer_init_growable(&writer, &data, &total);
  seria::serialize(a, &writer);
  auto result = mpack_writer_destroy(&writer);

  uint8_t target[] = {0x95, 0x1, 0x2, 0x3, 0x4, 0x5};
  REQUIRE(result == mpack_ok);
  REQUIRE(std::memcmp(data, target, total) == 0);

  free(data);
}

TEST_CASE("customize enum serialize rule", "[serialize]") {
  std::vector<Child> children{Child::Boy, Child::Girl, Child::Girl};

  char *data = nullptr;
  size_t total = 0;
  mpack_writer_t writer;
  mpack_writer_init_growable(&writer, &data, &total);
  seria::serialize(children, &writer);
  auto result = mpack_writer_destroy(&writer);

  uint8_t target[] = {0x93, 0xA1, 0x42, 0xA1, 0x47, 0xA1, 0x47};
  REQUIRE(result == mpack_ok);
  REQUIRE(std::memcmp(data, target, total) == 0);

  free(data);
}

TEST_CASE("deserialize c style array", "[deserialize]") {
  uint8_t data[] = {0x93, 0x01, 0x02, 0x03};
  int a[] = {0, 0, 0};

  mpack_tree_t tree;
  mpack_tree_init_data(&tree, reinterpret_cast<const char *>(data),
                       sizeof(data));
  mpack_tree_parse(&tree);
  mpack_node_t root = mpack_tree_root(&tree);
  seria::deserialize(a, root);

  REQUIRE((a[0] == 1 && a[1] == 2 && a[2] == 3));
}

TEST_CASE("deserialize std::array", "[deserialize]") {
  uint8_t data[] = {0x93, 0x01, 0x02, 0x03};
  array<int, 3> a = {0, 0, 0};

  mpack_tree_t tree;
  mpack_tree_init_data(&tree, reinterpret_cast<const char *>(data),
                       sizeof(data));
  mpack_tree_parse(&tree);
  mpack_node_t root = mpack_tree_root(&tree);

  seria::deserialize(a, root);
  REQUIRE((a[0] == 1 && a[1] == 2 && a[2] == 3));
}

TEST_CASE("deserialize nested object", "[deserialize]") {
  uint8_t data[] = {0x85, 0xA3, 0x61, 0x67, 0x65, 0x00, 0xA5, 0x76, 0x61, 0x6C,
                    0x75, 0x65, 0xCC, 0xE9, 0xA6, 0x67, 0x65, 0x6E, 0x64, 0x65,
                    0x72, 0x01, 0xA9, 0x74, 0x65, 0x73, 0x74, 0x5F, 0x75, 0x69,
                    0x6E, 0x74, 0x02, 0xA6, 0x69, 0x6E, 0x73, 0x69, 0x64, 0x65,
                    0x83, 0xA5, 0x69, 0x5F, 0x61, 0x67, 0x65, 0xCC, 0xE9, 0xA7,
                    0x69, 0x5F, 0x76, 0x61, 0x6C, 0x75, 0x65, 0xCB, 0x3F, 0xCD,
                    0xD2, 0xF1, 0xA9, 0xFB, 0xE7, 0x6D, 0xA3, 0x69, 0x5F, 0x76,
                    0x93, 0x06, 0x42, 0xCD, 0x02, 0x9A};

  Person person{100, 2.0f};

  mpack_tree_t tree;
  mpack_tree_init_data(&tree, reinterpret_cast<const char *>(data),
                       sizeof(data));
  mpack_tree_parse(&tree);
  mpack_node_t root = mpack_tree_root(&tree);
  seria::deserialize(person, root);

  REQUIRE(person.age == 0);
  REQUIRE(person.gender == Gender::Female);
  REQUIRE(person.value == 233.0f);
  REQUIRE(person.test_uint == 2);
  REQUIRE(person.inside.i_age == 233);
  REQUIRE(person.inside.i_value == 0.233f);
  REQUIRE((person.inside.i_v[0] == 6 && person.inside.i_v[1] == 66 &&
           person.inside.i_v[2] == 666));
}

TEST_CASE("deserialize a object with default value", "[deserialize]") {
  Person person{};

  uint8_t data[] = {0x83, 0xA5, 0x76, 0x61, 0x6C, 0x75, 0x65, 0xCB, 0x3F,
                    0xC9, 0x99, 0x99, 0x99, 0x99, 0x99, 0x9A, 0xA9, 0x74,
                    0x65, 0x73, 0x74, 0x5F, 0x75, 0x69, 0x6E, 0x74, 0x02,
                    0xA6, 0x69, 0x6E, 0x73, 0x69, 0x64, 0x65, 0x82, 0xA7,
                    0x69, 0x5F, 0x76, 0x61, 0x6C, 0x75, 0x65, 0xCB, 0x3F,
                    0xCD, 0xD2, 0xF1, 0xA9, 0xFB, 0xE7, 0x6D, 0xA3, 0x69,
                    0x5F, 0x76, 0x93, 0x06, 0x42, 0xCD, 0x02, 0x9A};

  mpack_tree_t tree;
  mpack_tree_init_data(&tree, reinterpret_cast<const char *>(data),
                       sizeof(data));
  mpack_tree_parse(&tree);
  mpack_tree_set_error_handler(&tree,
                               [](mpack_tree_t *tree, mpack_error_t error) {
                                 std::cout << error << std::endl;
                               });

  mpack_node_t root = mpack_tree_root(&tree);
  seria::deserialize(person, root);

  REQUIRE(person.age == 50);
  REQUIRE(person.value == 0.2f);
  REQUIRE(person.gender == Gender::Male);
  REQUIRE(person.inside.i_age == 100);
}

TEST_CASE("customize enum deserialize rule", "[deserialize]") {
  uint8_t data[] = {0x93, 0xA1, 0x42, 0xA1, 0x47, 0xA1, 0x47};
  std::vector<Child> children{};

  mpack_tree_t tree;
  mpack_tree_init_data(&tree, reinterpret_cast<const char *>(data),
                       sizeof(data));
  mpack_tree_parse(&tree);
  mpack_node_t root = mpack_tree_root(&tree);
  seria::deserialize(children, root);

  REQUIRE(children.size() == 3);
  REQUIRE(children[0] == Child::Boy);
  REQUIRE(children[1] == Child::Girl);
  REQUIRE(children[2] == Child::Girl);
}

TEST_CASE("integer to float", "[deserialize]") {
  uint8_t data[] = {0xD0, 0x9C}; // -100

  float value = 0.0f;
  mpack_tree_t tree;
  mpack_tree_init_data(&tree, reinterpret_cast<const char *>(data),
                       sizeof(data));
  mpack_tree_parse(&tree);
  mpack_node_t root = mpack_tree_root(&tree);
  seria::deserialize(value, root);

  REQUIRE(value == -100.0f);
}

TEST_CASE("float to int should fail", "[deserialize]") {
  uint8_t data[] = {0xCB, 0x3F, 0xF3, 0x33, 0x33,
                    0x33, 0x33, 0x33, 0x33}; // 1.2

  int value = 0;
  mpack_tree_t tree;
  mpack_tree_init_data(&tree, reinterpret_cast<const char *>(data),
                       sizeof(data));
  mpack_tree_parse(&tree);
  mpack_node_t root = mpack_tree_root(&tree);

  auto has_exception = false;
  try {
    seria::deserialize(value, root);
  } catch (seria::type_error &err) {
    REQUIRE(std::strcmp(err.desired_type(), "integer") == 0);
    has_exception = true;
  }
  REQUIRE(has_exception);
}

TEST_CASE("deserialize type error", "[deserialize]") {
  uint8_t data[] = {0x83, 0xA5, 0x76, 0x61, 0x6C, 0x75, 0x65, 0x01, 0xA9,
                    0x74, 0x65, 0x73, 0x74, 0x5F, 0x75, 0x69, 0x6E, 0x74,
                    0x02, 0xA6, 0x69, 0x6E, 0x73, 0x69, 0x64, 0x65, 0x82,
                    0xA7, 0x69, 0x5F, 0x76, 0x61, 0x6C, 0x75, 0x65, 0x01,
                    0xA3, 0x69, 0x5F, 0x76, 0x92, 0x01, 0xCB, 0x3F, 0xF3,
                    0x33, 0x33, 0x33, 0x33, 0x33, 0x33};
  Person person{};

  mpack_tree_t tree;
  mpack_tree_init_data(&tree, reinterpret_cast<const char *>(data),
                       sizeof(data));
  mpack_tree_parse(&tree);
  mpack_node_t root = mpack_tree_root(&tree);
  try {
    seria::deserialize(person, root);
  } catch (seria::type_error &err) {
    REQUIRE(std::strcmp(err.path(), "inside.i_v.1") == 0);
    REQUIRE(std::strcmp(err.desired_type(), "integer") == 0);
  }
}

TEST_CASE("deserialize missing value", "[deserialize]") {
  uint8_t data[] = {0x83, 0xA5, 0x76, 0x61, 0x6C, 0x75, 0x65, 0x01, 0xA9,
                    0x74, 0x65, 0x73, 0x74, 0x5F, 0x75, 0x69, 0x6E, 0x74,
                    0x02, 0xA6, 0x69, 0x6E, 0x73, 0x69, 0x64, 0x65, 0x81,
                    0xA3, 0x69, 0x5F, 0x76, 0x92, 0x01, 0x01};
  Person person{};

  mpack_tree_t tree;
  mpack_tree_init_data(&tree, reinterpret_cast<const char *>(data),
                       sizeof(data));
  mpack_tree_parse(&tree);
  mpack_node_t root = mpack_tree_root(&tree);
  try {
    seria::deserialize(person, root);
  } catch (seria::error &err) {
    REQUIRE(std::strcmp(err.path(), "inside.i_value") == 0);
  }
}