#define CATCH_CONFIG_MAIN
#define CATCH_CONFIG_FAST_COMPILE
#include "third_party/catch.hpp"
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>
#include <seria/deserialize.hpp>
#include <seria/serialize.hpp>

using namespace std;

enum class Gender { Male, Female };

struct Inside {
  int i_age = 1;
  float i_value = 1.0f;
  std::vector<int> i_v = {1, 2, 3, 4, 5};
};

struct Person {
  int age = 1;
  float value = 1.0f;
  Gender gener = Gender::Male;
  uint32_t test_uint = 1;
  Inside inside{};
};

namespace seria {

template <> auto register_object<Person>() {
  return std::make_tuple(member("age", &Person::age, std::make_unique<int>(50)),
                         member("value", &Person::value),
                         member<Person, Gender, const char *>(
                             "gender", &Person::gener,
                             std::make_unique<Gender>(Gender::Male),
                             [](const char *const &v) -> Gender {
                               if (std::strcmp(v, "M") == 0) {
                                 return Gender::Male;
                               }
                               return Gender::Female;
                             },
                             [](const Gender &v) -> const char * {
                               if (v == Gender::Male) {
                                 return "M";
                               }
                               return "F";
                             }),
                         member("test_uint", &Person::test_uint),
                         member("inside", &Person::inside));
}

template <> auto register_object<Inside>() {
  return std::make_tuple(
      member("i_age", &Inside::i_age, std::make_unique<int>(100)),
      member("i_value", &Inside::i_value), member("i_v", &Inside::i_v));
}

} // namespace seria

TEST_CASE("serialize simple arrays", "[serialize]") {
  {
    int a[] = {1, 2, 3, 4, 5};
    auto json = seria::serialize(a);
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    json.Accept(writer);
    REQUIRE(string("[1,2,3,4,5]") == buffer.GetString());
  }

  {
    array<int, 5> a = {1, 2, 3, 4, 5};
    auto json = seria::serialize(a);
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    json.Accept(writer);
    REQUIRE(string("[1,2,3,4,5]") == buffer.GetString());
  }

  {
    vector<int> a = {1, 2, 3, 4};
    a.push_back(5);
    auto json = seria::serialize(a);
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    json.Accept(writer);
    REQUIRE(string("[1,2,3,4,5]") == buffer.GetString());
  }
}

TEST_CASE("serialize a nested object", "[serialize]") {
  Person person{100, 2.0f};
  auto res = seria::serialize(person);
  rapidjson::StringBuffer buffer;
  rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
  res.Accept(writer);
  std::string target =
      R"({"age":100,"value":2.0,"gender":"M","test_uint":1,"inside":{"i_age":1,"i_value":1.0,"i_v":[1,2,3,4,5]}})";
  REQUIRE(target == buffer.GetString());
}

TEST_CASE("deserialize nested object", "[deserialize]") {
  Person person{100, 2.0f};

  const char *str =
      R"({"age":0,"value":233.0,"gender":"F","test_uint":2,"inside":{"i_age":233,"i_value":0.233,"i_v":[6,66,666]}})";

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
      R"({"age":1,"value":1.0,"gender":"M","test_uint":1,"inside":{"i_age":1,"i_value":1.0,"i_v":[1,2,3,4,5]}})";

  REQUIRE(str == target);
}