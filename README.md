# seria
Serialize/deserialize C++ data structure to/from JSON, no RTTI.

Support
- Array & Object
- Nested data structure
- Customize label for object member
- Default value during deserialize
- Transformer for enum value

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
  return std::make_tuple(member("value", &Test::value, std::make_unique<int>(666)), // default value
                         member("inside", &Test::inside)); // nested object
}

template <> auto register_object<Inside>() {
  return std::make_tuple(member("a_value", &Inside::value), // customize label
                         member("arr", &Inside::arr)); // array
}
} // namespace seria

Test obj {};
auto json = seria::serialize(obj);

rapidjson::StringBuffer buffer;
rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
json.Accept(writer);

cout << buffer.GetString() << endl; // {"value":233,"inside":{"a_value":1.0,"arr":[1,2,3]}}
``` 
