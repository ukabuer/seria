#include <catch2/catch_all.hpp>
#include <seria/deserialize/rapidjson.hpp>
#include <seria/serialize/rapidjson.hpp>

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

class User {
private:
  Gender gender = Gender::Male;
public:
  int age = 1;
  double height = 180.0;
  double value = 1.0;
  
  Gender get_gender() const {return gender;}
  bool set_gender(const Gender& new_gender)  {
    gender = new_gender; 
    return gender == Gender::Male;
  }
  double get_value() const {return value;}
  void set_value(double new_value)  { value = new_value;}
  
  double get_height() const {return height;}
  void set_height(double new_value)  { height = new_value;}
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

template <> auto register_object<User>() {
  return std::make_tuple(member("age", &User::age, 32),
                         member("gender", &User::set_gender,  &User::get_gender, Gender::Female),
                         member("value", &User::set_value, &User::get_value),
                         member("height", &User::set_height, &User::get_height, 175.5));
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
    throw type_error("", "should be string");
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

TEST_CASE("customize enum serialize rule", "[serialize]") {
  std::vector<Child> children{Child::Boy, Child::Girl, Child::Girl};
  auto str = seria::to_string(children);
  std::string target = R"(["B","G","G"])";

  REQUIRE(str == target);
}

TEST_CASE("string", "[serialize]") {
  std::string value = "hello";
  auto str = seria::to_string(value);
  REQUIRE(str == R"("hello")");
}

TEST_CASE("stringify a person object", "[to_string]") {
  Person person{};
  auto str = seria::to_string(person);
  std::string target =
      R"({"age":1,"value":1.0,"gender":0,"test_uint":1,"inside":{"i_age":1,"i_value":1.0,"i_v":[1,2,3,4,5]}})";

  REQUIRE(str == target);
}

TEST_CASE("deserialize c style array", "[deserialize]") {
  const char *str = "[1,2,3]";
  int a[] = {0, 0, 0};

  rapidjson::Document document;
  document.Parse(str);
  seria::deserialize(a, document);

  auto res = seria::to_string(a);
  REQUIRE(res == "[1,2,3]");
}

TEST_CASE("deserialize std::array", "[deserialize]") {
  const char *str = "[1,2,3]";
  array<int, 3> a = {0, 0, 0};

  rapidjson::Document document;
  document.Parse(str);
  seria::deserialize(a, document);

  auto res = seria::to_string(a);
  REQUIRE(res == "[1,2,3]");
}

TEST_CASE("deserialize nested object", "[deserialize]") {
  Person person{100, 2.0f};

  const char *str =
      R"({"age":0,"value":233.0,"gender":1,"test_uint":2,"inside":{"i_age":233,"i_value":0.233,"i_v":[6,66,666]}})";

  rapidjson::Document document;
  document.Parse(str);
  seria::deserialize(person, document);

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

  const char *str =
      R"({"value":233.0,"test_uint":2,"inside":{"i_value":0.233,"i_v":[6,66,666]}})";

  rapidjson::Document document;
  document.Parse(str);
  seria::deserialize(person, document);

  REQUIRE(person.age == 50);
  REQUIRE(person.value == 233.0f);
  REQUIRE(person.gender == Gender::Male);
  REQUIRE(person.inside.i_age == 100);
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

TEST_CASE("integer to float", "[deserialize]") {
  std::string input = "-100";

  rapidjson::Document json;
  json.Parse(input.c_str());

  float data = 0.0f;
  seria::deserialize(data, json);

  REQUIRE(data == -100.0f);
}

TEST_CASE("float to int should fail", "[deserialize]") {
  std::string input = "-100.0";

  rapidjson::Document json;
  json.Parse(input.c_str());

  int data = 0;
  auto has_exception = false;
  try {
    seria::deserialize(data, json);
  } catch (seria::type_error &err) {
    REQUIRE(std::strcmp(err.desired_type(), "integer") == 0);
    has_exception = true;
  }
  REQUIRE(has_exception);
}

TEST_CASE("deserialize type error", "[deserialize]") {
  Person person{};
  auto str =
      R"({"value":1,"test_uint":2,"inside":{"i_value":1,"i_v":[1,1.0]}})";

  rapidjson::Document json;
  json.Parse(str);
  try {
    seria::deserialize(person, json);
  } catch (seria::type_error &err) {
    REQUIRE(std::strcmp(err.path(), "inside.i_v.1") == 0);
    REQUIRE(std::strcmp(err.desired_type(), "integer") == 0);
  }
}

TEST_CASE("deserialize missing value", "[deserialize]") {
  Person person{};
  auto str = R"({"value":1,"test_uint":2,"inside":{"i_v":[1,1.0]}})";

  rapidjson::Document json;
  json.Parse(str);
  try {
    seria::deserialize(person, json);
  } catch (seria::error &err) {
    REQUIRE(std::strcmp(err.path(), "inside.i_value") == 0);
  }
}


TEST_CASE("stringify an object contains private fields", "[to_string]") {
  User user{};
  user.value = 3.6;
  user.age = 27;
  
  auto str = seria::to_string(user);
  std::string target =
      R"({"age":27,"gender":0,"value":3.6,"height":180.0})";
  
  REQUIRE(str == target);
}

TEST_CASE("deserialize to an object with private fields", "[deserialize]") {
  User user{};
  
  std::string target =  R"({"value":3.6})";
  rapidjson::Document document;
  document.Parse(target.c_str());
  seria::deserialize(user, document);
  const static double error = 0.0000001;
  REQUIRE((user.value > 3.6 - error && user.value < 3.6 + error));
  REQUIRE(user.age == 32);
}
