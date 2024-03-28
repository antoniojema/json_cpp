#pragma once

#include "utils.h"

#include <string>
#include <vector>
#include <map>
#include <optional>
#include <variant>
#include <type_traits>

namespace JSON {

using Bool = bool;

using Integer = long long int;
using Float = double;

class Value;

using Null = struct {};
using String = std::string;
using Array = std::vector<Value>;
using Object = std::map<String, Value>;

constexpr Null null {};

using Number = ::Number<Integer, Float>;

enum class ValueType {
    Null = 0, Bool, Number, String, Array, Object
};

template <typename T>
concept ValidValue = ::IsAnyOf<T,
    Null,
    Bool,
    Number,
    String,
    Array,
    Object
>;

template <typename T>
concept ValidNumber = ::IsAnyOf<T,
    Number::Integer,
    Number::Float
>;

class Value {
public:
    Value() : _value(null) {}

    template<::Integral T> Value(T val) : _value(Number((Integer)val)) {}
    template<::Floating T> Value(T val) : _value(Number((Float)val)) {}
    Value(const std::string_view& val) : _value(String(val)) {}
    Value(const char * const val) : _value(String(val)) {}

    template<ValidValue T> Value(const T& val) : _value(val) {}
    template<ValidValue T> Value(T&& val) noexcept : _value(std::move(val)) {}

    ValueType type() const {
        return (ValueType)this->_value.index();
    }
    bool is_null() const { return this->type() == ValueType::Null; }

    Value& get_array_elem(size_t i) {
        return std::get<Array>(this->_value)[i];
    }
    const Value& get_array_elem(size_t i) const {
        return std::get<Array>(this->_value)[i];
    }

    Value& get_object_elem(const String& str) {
        return std::get<Object>(this->_value).at(str);
    }
    const Value& get_object_elem(const String& str) const {
        return std::get<Object>(this->_value).at(str);
    }

    Value& get_or_emplace_object_elem(const String& str) {
        return std::get<Object>(this->_value)[str];
    }

    template<ValidValue T> T& get() {
        return std::get<T>(this->_value);
    }
    template<ValidValue T> const T& get() const {
        return std::get<T>(this->_value);
    }

    template<ValidNumber T> T& get() {
        return this->get<Number>().get<T>();
    }
    template<ValidNumber T> const T& get() const {
        return this->get<Number>().get<T>();
    }

private:
    std::variant<Null, Bool, Number, String, Array, Object> _value;
};

std::string to_string(const Number& val);
std::string to_string(const Bool  & val);
std::string to_string(const String& val);
std::string to_string(const Array & val, bool endl=true, size_t tab_size=2, size_t tab_ini=0);
std::string to_string(const Object& val, bool endl=true, size_t tab_size=2, size_t tab_ini=0);
std::string to_string(const Value & val, bool endl=true, size_t tab_size=2, size_t tab_ini=0);

std::optional<Value> try_parse(std::string_view content);
// Someday I should make it work with any std::istream&, copying the content of a file to a string is expensive

} // namespace JSON
