#include "json.h"

#include <regex>
#include <optional>
#include <iostream>

#define SPACELIKE " \t\n\r"


namespace JSON {

namespace {

using MatchResults = std::match_results<std::string_view::iterator>;
struct NullMatch {std::string_view::iterator end;};
struct BoolMatch {Bool value; std::string_view::iterator end;};
struct NumberMatch {Number value; std::string_view::iterator end;};
struct StringMatch {String value; std::string_view::iterator end;};
struct ArrayMatch {Array value; std::string_view::iterator end;};
struct ObjectMatch {Object value; std::string_view::iterator end;};
struct ValueMatch {Value value; std::string_view::iterator end;};

const std::regex re_float   {R"(^\-?([1-9][0-9]*|0)(\.[0-9]+)?([eE][\-\+]?[0-9]+)?)"};
const std::regex re_integer {R"(^\-?([1-9][0-9]*|0)(?=$|[^.eE0123456789]))"};
//const std::regex re_string  {R"(^"([^\\"]|\\(["\\\/bfnrt]|(u([0-9]|[a-f]|[A-F]){4})))*")"};

std::string_view moveToNonEmpty(std::string_view content) {
    return {content.begin() + content.find_first_not_of(SPACELIKE), content.end()};
}

std::optional<NullMatch> try_parse_null(std::string_view content) {
    content = moveToNonEmpty(content);
    if (content.starts_with("null")) return NullMatch{content.begin()+4};
    return std::nullopt;
}

std::optional<BoolMatch> try_parse_bool(std::string_view content) {
    content = moveToNonEmpty(content);
    if (content.starts_with("true" )) return BoolMatch{true , content.begin()+4};
    if (content.starts_with("false")) return BoolMatch{false, content.begin()+5};
    return std::nullopt;
}

std::optional<NumberMatch> try_parse_integer(std::string_view content) {
    content = moveToNonEmpty(content);
    MatchResults match_results;
    if (!std::regex_search(content.begin(), content.end(), match_results, re_integer)) {
        return std::nullopt;
    }
    std::string value_str = {match_results[0].first, match_results[0].second};
    return NumberMatch{std::atoll(value_str.c_str()), match_results[0].second};
}

std::optional<NumberMatch> try_parse_float(std::string_view content) {
    content = moveToNonEmpty(content);
    MatchResults match_results;
    if (!std::regex_search(content.begin(), content.end(), match_results, re_float)) {
        return std::nullopt;
    }
    std::string value_str = {match_results[0].first, match_results[0].second};
    return NumberMatch{std::atof(value_str.c_str()), match_results[0].second};
}

std::optional<NumberMatch> try_parse_number(std::string_view content) {
    auto integer = try_parse_integer(content);
    if (integer.has_value()) return integer;

    auto floating = try_parse_float(content);
    if (floating.has_value()) return floating;

    return std::nullopt;
}

std::optional<StringMatch> try_parse_string(std::string_view content) {
    content = moveToNonEmpty(content);

    if (content[0] != '\"') return std::nullopt;
    size_t n = 1;
    while (true) {
        if (content[n] == '\"') break;
        else if (content[n] == '\\') {
            switch (content[n+1]) {
            case '\"':
            case '\\':
            case '/':
            case 'b':
            case 'n':
            case 'r':
            case 't':
                n += 2;
                break;
            case 'u':
                for (size_t n2 = n+2; n2 <n+6; ++n2) {
                    char c = content[n2];
                    if (!(
                        (c >= '0' && c <= '9') ||
                        (c >= 'a' && c <= 'f') ||
                        (c >= 'A' && c <= 'F')
                    )) {
                        return std::nullopt;
                    }
                }
                n += 6;
                break;
            default:
                return std::nullopt;
            }
        }
        else ++n;
    }
    
    return StringMatch{
        std::string(content.substr(1,n-1)),
        content.begin()+(n+1)
    };

    //MatchResults match_results;
    //if (!std::regex_search(content.begin(), content.end(), match_results, re_string)) {
    //    return std::nullopt;
    //}
    //std::string string = {match_results[0].first+1, match_results[0].second-1};
    //return StringMatch{string, match_results[0].second};
}

std::optional<ValueMatch> try_parse_value(std::string_view content);

std::optional<ArrayMatch> try_parse_array(std::string_view content) {
    auto old_content = content;
    content = moveToNonEmpty(content);
    if (content[0] != '[') return std::nullopt;
    content = moveToNonEmpty({ content.begin() + 1, content.end() });
    Array arr {};
    while (true) {
        content = moveToNonEmpty(content); if (content.empty()) return std::nullopt;
        if (content[0] == ']') break;

        auto value = try_parse_value(content);
        if (!value.has_value()) return std::nullopt;
        arr.emplace_back(std::move(value->value));

        content = moveToNonEmpty({&*value->end, &*(content.end()-1)+1}); if (content.empty()) return std::nullopt;
        if (content[0] == ',') {
            content = moveToNonEmpty({content.begin()+1, content.end() }); if (content.empty()) return std::nullopt;
        }
        else if (content[0] == ']') {
            break;
        }
        else {
            return std::nullopt;
        }
    }

    return ArrayMatch{std::move(arr), content.begin()+1};
}

std::optional<ObjectMatch> try_parse_object(std::string_view content) {
    content = moveToNonEmpty(content);
    if (content[0] != '{') return std::nullopt;
    content = moveToNonEmpty({content.begin()+1, content.end()});
    Object obj{};
    while (true) {
        content = moveToNonEmpty(content); if (content.empty()) return std::nullopt;
        if (content[0] == '}') break;

        auto key = try_parse_string(content);
        if (!key.has_value()) return std::nullopt;

        content = moveToNonEmpty({key->end, content.end()}); if (content.empty()) return std::nullopt;
        if (content[0] != ':') return std::nullopt;
        
        content = moveToNonEmpty({content.begin()+1, content.end()}); if (content.empty()) return std::nullopt;
        auto value = try_parse_value(content);
        if (!value.has_value()) {std::cout << "Failed on key " << key->value << '\n'; return std::nullopt; }
        obj.insert({std::move(key->value), std::move(value->value)});
        
        content = moveToNonEmpty({&*value->end, &*(content.end()-1)+1}); if (content.empty()) return std::nullopt;
        if (content[0] == ',') {
            content = moveToNonEmpty({content.begin()+1, content.end()}); if (content.empty()) return std::nullopt;
        }
        else if (content[0] == '}') {
            break;
        }
        else {
            return std::nullopt;
        }
    }

    return ObjectMatch{std::move(obj), content.begin()+1};
}

std::optional<ValueMatch> try_parse_value(std::string_view content) {
    auto null = try_parse_null(content);
    if (null.has_value()) return ValueMatch{::JSON::null, null->end};

    auto boolean = try_parse_bool(content);
    if (boolean.has_value()) return ValueMatch{boolean->value, boolean->end};

    auto number = try_parse_number(content);
    if (number.has_value()) return ValueMatch{number->value, number->end};

    auto string = try_parse_string(content);
    if (string.has_value()) return ValueMatch{string->value, string->end};

    auto arr = try_parse_array(content);
    if (arr.has_value()) return ValueMatch{std::move(arr->value), arr->end};

    auto obj = try_parse_object(content);
    if (obj.has_value()) return ValueMatch{std::move(obj->value), obj->end};

    return std::nullopt;
}

} // namespace

std::optional<Value> try_parse(std::string_view content) {
    auto json = try_parse_value(content);
    return json.has_value()
        ? std::optional<Value>{std::move(json->value)}
        : std::nullopt;
}

} // namespace JSON 