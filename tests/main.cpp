#include <seria/deserialize.hpp>
#include <seria/serialize.hpp>
#define CATCH_CONFIG_MAIN
#define CATCH_CONFIG_FAST_COMPILE
#include "third_party/catch.hpp"
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>

using namespace std;

struct Inside {
  int i_age = 1;
  float i_value = 0.111;
  std::vector<int> i_v = {1, 2, 3, 4, 5};
};

struct Person {
  int age = 0;
  float value = 3.333;
  Inside inside{};
};

namespace seria {

template <> auto registerObject<Person>() {
  return std::make_tuple(member("age", &Person::age),
                         member("value", &Person::value),
                         member("inside", &Person::inside));
}

template <> auto registerObject<Inside>() {
  return std::make_tuple(member("i_age", &Inside::i_age),
                         member("i_value", &Inside::i_value),
                         member("i_v", &Inside::i_v));
}

} // namespace seria

TEST_CASE("simple array", "[array]") {
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

TEST_CASE("nested object", "serialize, deserialize") {
  Person person{100, 1.2345};
  {
    auto res = seria::serialize(person);
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    res.Accept(writer);
    cout << buffer.GetString() << endl;
  }

  const char *str =
      R"({"age":0,"value":233.0,"inside":{"i_age":233,"i_value":0.233,"i_v":[6,66,666]}})";

  rapidjson::Document document;
  document.Parse(str);
  seria::deserialize(person, document);

  REQUIRE(person.age == 0);
  REQUIRE(person.value == 233.0f);
  REQUIRE(person.inside.i_age == 233);
}