#include <json/json.h>

#include <iostream>
#include <assert.h>

int main() {
    JSON::Object json {
        {"this is", "a json file"},
        {"an integer", 42},
        {"a float", 3.14},
        {"a boolean", true},
        {"this is true", false},
        {"a secret", JSON::null},
        {"a list", JSON::Array{
            1,
            2,
            3,
            JSON::Array{
                4,
                5,
                "pretty deep",
                JSON::Object{
                    {"secret", 69}
                }
            }
        }},
        {"a dict", JSON::Object{
            {"key", "value"},
            {"another key", "another value"},
            {"a list", JSON::Array{
                1,
                2,
                3
            }}
        }}
    };

    std::cout << "JSON content with line endings:\n"
              << JSON::to_string(json) << '\n';

    std::cout << "JSON content with greater tabulations:\n"
              << JSON::to_string(json, true, 6) << '\n';

    std::cout << "JSON content without line endings:\n"
              << JSON::to_string(json, false) << '\n';

    auto& integer = json.at("an integer");
    
    assert(integer.type() == JSON::ValueType::Number);
    auto& as_number = integer.get<JSON::Number>();

    assert(as_number.is_integer());

    auto& secret = json
        .at("a list")
        .get_array_elem(3)
        .get_array_elem(3)
        .get_object_elem("secret");
    
    assert(secret.type() == JSON::ValueType::Number);

    std::cout << "The secret: " << secret.get<JSON::Number::Integer>() << '\n';
}
