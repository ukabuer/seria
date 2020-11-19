# seria
Serialize/deserialize C++ data structure to/from JSON, no RTTI.

Support
- boolean, integer, float/double, enum, std::string/C string, std::array/std::vector/C array, and struct/class
- nested data structure
- customize label for object member
- default value during deserialize
- customize rule for enum/object

Usage:
```c++
struct Inside {
  float value = 1.0f;
  std::vector<int> arr = {1, 2, 3};
};

struct Test {
  int value = 233;
  Inside inside;
};

// register your object
namespace seria {
template <> auto register_object<Test>() {
  return std::make_tuple(member("value", &Test::value, 666), // default value
                         member("inside", &Test::inside)); // nested object
}

template <> auto register_object<Inside>() {
  return std::make_tuple(member("a_value", &Inside::value), // customize label
                         member("arr", &Inside::arr)); // array
}
} // namespace seria

Test obj {};
auto str = seria::to_string(obj);
// {"value":233,"inside":{"a_value":1.0,"arr":[1,2,3]}}

auto json = seria::serialize(obj);
Test data {};
seria::deserialize(data, json);
``` 
