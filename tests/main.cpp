#define CATCH_CONFIG_MAIN
#define CATCH_CONFIG_FAST_COMPILE
#include "third_party/catch.hpp"
#include <seria/deserialize.hpp>
#include <seria/serialize.hpp>

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
  Gender gener = Gender::Male;
  uint32_t test_uint = 1;
  Inside inside{};
};

enum class Child { Boy, Girl };

namespace seria {

template <> auto register_object<Person>() {
  return std::make_tuple(member("age", &Person::age, 50),
                         member("value", &Person::value),
                         member("gender", &Person::gener, Gender::Male),
                         member("test_uint", &Person::test_uint),
                         member("inside", &Person::inside));
}

template <> auto register_object<Inside>() {
  return std::make_tuple(member("i_age", &Inside::i_age, 100),
                         member("i_value", &Inside::i_value),
                         member("i_v", &Inside::i_v));
}

template <> rapidjson::Document serialize(const Child &data) {
  rapidjson::Document json(rapidjson::kStringType);
  if (data == Child::Boy) {
    json.SetString("B");
  } else {
    json.SetString("G");
  }
  return json;
}

template <> void deserialize(Child &data, const rapidjson::Value &json) {
  if (!json.IsString()) {
    throw std::runtime_error("invalid data type in json, should be string");
  }

  if (std::strcmp("B", json.GetString()) == 0) {
    data = Child::Boy;
  } else {
    data = Child::Girl;
  }
}

} // namespace seria

namespace seria {} // namespace seria

TEST_CASE("serialize c style array", "[serialize]") {
  int a[] = {1, 2, 3, 4, 5};
  auto str = seria::to_string(a);
  REQUIRE(str == "[1,2,3,4,5]");
}

TEST_CASE("serialize std::array", "[serialize]") {
  array<int, 5> a = {1, 2, 3, 4, 5};
  auto str = seria::to_string(a);
  REQUIRE(str == "[1,2,3,4,5]");
}

TEST_CASE("serialize std::vector", "[serialize]") {
  vector<int> a = {1, 2, 3, 4};
  a.push_back(5);
  auto str = seria::to_string(a);
  REQUIRE(str == "[1,2,3,4,5]");
}

TEST_CASE("serialize a nested object", "[serialize]") {
  Person person{100, 2.0f};
  auto res = seria::to_string(person);
  std::string target =
      R"({"age":100,"value":2.0,"gender":0,"test_uint":1,"inside":{"i_age":1,"i_value":1.0,"i_v":[1,2,3,4,5]}})";
  REQUIRE(target == res);
}

TEST_CASE("deserialize nested object", "[deserialize]") {
  Person person{100, 2.0f};

  const char *str =
      R"({"age":0,"value":233.0,"gender":1,"test_uint":2,"inside":{"i_age":233,"i_value":0.233,"i_v":[6,66,666]}})";

  rapidjson::Document document;
  document.Parse(str);
  seria::deserialize(person, document);

  REQUIRE(person.age == 0);
  REQUIRE(person.gener == Gender::Female);
  REQUIRE(person.value == 233.0f);
  REQUIRE(person.test_uint == 2);
  REQUIRE(person.inside.i_age == 233);
  REQUIRE(person.inside.i_value == 0.233f);
  REQUIRE((person.inside.i_v[0] == 6 && person.inside.i_v[1] == 66 &&
           person.inside.i_v[2] == 666));
}

TEST_CASE("deserialize a object with default value", "[deserialize]") {
  Person person{};

  const char *str =
      R"({"value":233.0,"test_uint":2,"inside":{"i_value":0.233,"i_v":[6,66,666]}})";

  rapidjson::Document document;
  document.Parse(str);
  seria::deserialize(person, document);

  REQUIRE(person.age == 50);
  REQUIRE(person.value == 233.0f);
  REQUIRE(person.gener == Gender::Male);
  REQUIRE(person.inside.i_age == 100);
}

TEST_CASE("stringify a person object", "[to_string]") {
  Person person{};
  auto str = seria::to_string(person);
  std::string target =
      R"({"age":1,"value":1.0,"gender":0,"test_uint":1,"inside":{"i_age":1,"i_value":1.0,"i_v":[1,2,3,4,5]}})";

  REQUIRE(str == target);
}

TEST_CASE("customize enum serialize rule", "[serialize]") {
  std::vector<Child> children{Child::Boy, Child::Girl, Child::Girl};
  auto str = seria::to_string(children);
  std::string target = R"(["B","G","G"])";

  REQUIRE(str == target);
}

TEST_CASE("customize enum deserialize rule", "[deserialize]") {
  std::vector<Child> children{};
  std::string target = R"(["B","G","G"])";

  rapidjson::Document doc;
  doc.Parse(target.c_str());
  seria::deserialize(children, doc);

  REQUIRE(children.size() == 3);
  REQUIRE(children[0] == Child::Boy);
  REQUIRE(children[1] == Child::Girl);
  REQUIRE(children[2] == Child::Girl);
}