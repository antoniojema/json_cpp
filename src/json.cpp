#include "json.h"

namespace JSON {

namespace {

std::string impl_get_string(const Value& val, bool endl, size_t tab_size, size_t tab_ini);

std::string impl_get_string(Null) { return "null"; }

std::string impl_get_string(Bool val) { return val? "true" : "false"; }

std::string impl_get_string(Integer val) { return std::to_string(val); }
std::string impl_get_string(Float val) { return std::to_string(val); }

std::string impl_get_string(const Number& val) {
    return val.is_integer()
        ? impl_get_string(val.get<Integer>())
        : impl_get_string(val.get<Float>());
}

std::string impl_get_string(const String& val) { return "\"" + val + "\""; }

std::string impl_get_string(const Array& val, bool endl, size_t tab_size, size_t tab_ini) {
    std::string ret_str = "[";
    for (size_t i = 0; i < val.size(); ++i) {
        if (endl) ret_str += '\n' + std::string(tab_ini + tab_size, ' ');
        ret_str += impl_get_string(val[i], endl, tab_size, tab_ini+tab_size);
        if (i < val.size()-1) ret_str += ",";
    }
    if (endl) ret_str += '\n' + std::string(tab_ini, ' ');
    ret_str += ']';
    return ret_str;
}

std::string impl_get_string(const Object& val, bool endl, size_t tab_size, size_t tab_ini) {
    std::string ret_str = "{";
    const size_t n_elems = val.size();
    size_t i = 0;
    for (auto& elem : val) {
        if (endl) ret_str += '\n'+std::string(tab_ini+tab_size, ' ');
        ret_str += impl_get_string(elem.first) + ": " + impl_get_string(elem.second, endl, tab_size, tab_ini + tab_size);
        if (i++ < n_elems - 1) ret_str += ",";
    }
    if (endl) ret_str += '\n' + std::string(tab_ini, ' ');
    ret_str += '}';
    return ret_str;
}

std::string impl_get_string(const Value& val, bool endl, size_t tab_size, size_t tab_ini) {
    switch (val.type()) {
    case (ValueType::Null  ): return impl_get_string(val.get<Null  >());
    case (ValueType::Bool  ): return impl_get_string(val.get<Bool  >());
    case (ValueType::Number): return impl_get_string(val.get<Number>());
    case (ValueType::String): return impl_get_string(val.get<String>());
    case (ValueType::Array ): return impl_get_string(val.get<Array >(), endl, tab_size, tab_ini);
    case (ValueType::Object): return impl_get_string(val.get<Object>(), endl, tab_size, tab_ini);
    default: throw std::runtime_error("Invalid valuetype in impl_get_string: " + std::to_string((int)val.type()));
    }
}

} // namespace

std::string to_string(const Null  & val) {return impl_get_string(val);}
std::string to_string(const Bool  & val) {return impl_get_string(val);}
std::string to_string(const Number& val) {return impl_get_string(val);}
std::string to_string(const String& val) {return impl_get_string(val);}
std::string to_string(const Array & val, bool endl, size_t tab_size, size_t tab_ini) {return impl_get_string(val, endl, tab_size, tab_ini);}
std::string to_string(const Object& val, bool endl, size_t tab_size, size_t tab_ini) {return impl_get_string(val, endl, tab_size, tab_ini);}
std::string to_string(const Value & val, bool endl, size_t tab_size, size_t tab_ini) {return impl_get_string(val, endl, tab_size, tab_ini);}

} // namespace JSON
