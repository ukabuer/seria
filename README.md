# seria
Serialize/deserialize C++ data structure to/from JSON, no RTTI.

Support
- Array & Object
- Nested data structure
- Customize label for object member
- Default value during deserialize

Usage:
```c++
struct A {
  float value = 1.0f;
  std::vector<int> arr = {1, 2, 3};
};
struct B {
  int value = 233;
  A a;
};

// register your object
namespace seria {
template <> auto registerObject<B>() {
  return std::make_tuple(member("value", &B::value),
                         member("a", &B::a));
}

template <> auto registerObject<A>() {
  return std::make_tuple(member("a_value", &A::value), // customize label
                         member("arr", &A::arr));
}
} // namespace seria

B obj {};
auto json = seria::serialize(obj);

rapidjson::StringBuffer buffer;
rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
json.Accept(writer);

cout << buffer.GetString() << endl; // {"value":233,"a":{"a_value":1.0,"arr":[1,2,3]}}
``` 
