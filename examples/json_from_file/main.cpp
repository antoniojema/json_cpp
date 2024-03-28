#include "json.h"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <assert.h>

std::string get_content(const std::filesystem::path& filepath) {
    std::ifstream fin{filepath};
    std::ostringstream oss;
    oss << fin.rdbuf();
    return oss.str();
}

int main() {
    auto file = std::filesystem::path(__FILE__).parent_path() / "example.json";
    auto content = get_content(file);

    auto json_opt = JSON::try_parse(content);

    if (!json_opt) {
        std::cout << "There was an error!\n";
        return 0;
    }

    auto& json = *json_opt;
    std::cout << "JSON content with line endings:\n"
              << JSON::to_string(json) << '\n';

    std::cout << "JSON content with greater tabulations:\n"
              << JSON::to_string(json, true, 6) << '\n';

    std::cout << "JSON content without line endings:\n"
              << JSON::to_string(json, false) << '\n';

    assert(json.type() == JSON::ValueType::Object);
    auto& integer = json.get_object_elem("an integer");
    
    assert(integer.type() == JSON::ValueType::Number);
    auto& as_number = integer.get<JSON::Number>();

    assert(as_number.is_integer());

    auto& secret = json
        .get_object_elem("a list")
        .get_array_elem(3)
        .get_array_elem(3)
        .get_object_elem("secret");
    
    assert(secret.type() == JSON::ValueType::Number);

    std::cout << "The secret: " << secret.get<JSON::Number::Integer>() << '\n';
}
