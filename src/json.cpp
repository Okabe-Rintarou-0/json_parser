#include <cassert>
#include <charconv>
#include <iomanip>
#include "json.h"
#include "utils.h"

namespace json {
    std::string JSONObject::stringify(size_t indent) {
        std::ostringstream buf;
        _stringify(buf, indent, 0);
        return buf.str();
    }

    void JSONObject::_stringify(std::ostringstream &buf, size_t indent, size_t depth) const {
        switch (type()) {
            case JSON_NULL_T:
                buf << "null";
                break;
            case JSON_INT_T:
                buf << std::to_string(std::get<int>(m_value));
                break;
            case JSON_DOUBLE_T:
                buf << std::to_string(std::get<double>(m_value));
                break;
            case JSON_BOOL_T:
                buf << std::boolalpha << std::get<bool>(m_value);
                break;
            case JSON_LIST_T: {
                std::string indent_parent = std::string(depth * indent, ' ');
                std::string indent_son = std::string((depth + 1) * indent, ' ');
                buf << "[";
                const auto &list = std::get<json_list_t>(m_value);
                size_t list_len = list.size();
                if (!list.empty()) {
                    buf << '\n' << indent_son;
                    list[0]._stringify(buf, indent, depth + 1);
                    for (size_t i = 1; i < list_len; i++) {
                        buf << ",\n" << indent_son;
                        list[i]._stringify(buf, indent, depth + 1);
                    }
                }
                buf << '\n' << indent_parent << ']';
                break;
            }
            case JSON_DICT_T: {
                std::string indent_parent = std::string(depth * indent, ' ');
                std::string indent_son = std::string((depth + 1) * indent, ' ');
                buf << '{';
                const auto &dict = std::get<json_dict_t>(m_value);
                if (!dict.empty()) {
                    auto iter = dict.begin();
                    buf << '\n' << indent_son << '"' << iter->first << "\": ";
                    iter->second._stringify(buf, indent, depth + 1);
                    iter++;
                    for (; iter != dict.end(); iter++) {
                        buf << ",\n" << indent_son << '"' << iter->first << "\": ";
                        iter->second._stringify(buf, indent, depth + 1);
                    }
                }
                buf << '\n' << indent_parent << '}';
                break;
            }
            case JSON_STRING_T:
                buf << std::quoted(std::get<std::string>(m_value));
                break;
        }
    }

    JSONObject JSONObject::parse(std::string_view json) {
        auto [obj, _] = inner_parse(json);
        JSONType type = obj.type();
        if (type != JSON_LIST_T && type != JSON_DICT_T) {
            throw JSONParseException("Missing opening brace or square bracket.");
        }
        return obj;
    }

    char JSONObject::unescaped_char(char c) {
        switch (c) {
            case 'n':
                return '\n';
            case 't':
                return '\t';
            case 'r':
                return '\r';
            default:
                return c;
        }
    }

    std::pair<JSONObject, size_t> JSONObject::inner_parse(std::string_view json) {
        size_t eaten_blanks = count_blanks(json.data());
        if (eaten_blanks == json.length()) {
            throw JSONParseException("Invalid JSON, no content.");
        }

        json = std::string_view(json.data() + eaten_blanks);
//        std::cout << "json: " << json << std::endl;

        char c;
        JSONObject obj;
        size_t eaten{};

        c = json[0];
        if (std::isdigit(c)) {
            std::tie(obj, eaten) = try_parse_number(json);
        } else if (c == 'n') {
            std::tie(obj, eaten) = try_parse_null(json);
        } else if (c == 't' || c == 'f') {
            std::tie(obj, eaten) = try_parse_bool(json);
        } else if (c == '[') {
            std::tie(obj, eaten) = try_parse_list(json);
        } else if (c == '"') {
            std::tie(obj, eaten) = try_parse_string(json);
        } else if (c == '{') {
            std::tie(obj, eaten) = try_parse_dict(json);
        }

        eaten_blanks += count_blanks(json.data() + eaten);
        return {obj, eaten_blanks + eaten};
    }

    JSONObject::JSONObject(json::value_t &&v) : m_value(v) {}

    JSONObject::JSONObject() : JSONObject(nullptr) {}

    std::pair<JSONObject, size_t> JSONObject::try_parse_number(std::string_view json) {
        double decimal{};
        int integral{};

        auto result = std::from_chars(json.data(), json.data() + json.length(), decimal);
        if (result.ec == std::errc{}) {
            return {JSONObject{decimal}, result.ptr - json.data()};
        }

        result = std::from_chars(json.data(), json.data() + json.length(), integral);
        if (result.ec == std::errc{}) {
            return {JSONObject{integral}, result.ptr - json.data()};
        }

//        std::error_code ec = std::make_error_code(result.ec);
        throw JSONParseException("Error occurs when parsing a number");
    }

    size_t JSONObject::count_blanks(const char *json) {
        const char *str = json;
        while (std::isspace(*str)) {
            str++;
        }
        return str - json;
    }

    std::ostream &operator<<(std::ostream &os, const JSONObject &obj) {
        auto type = JSONType(obj.m_value.index());
        switch (type) {
            case JSON_BOOL_T:
                os << std::boolalpha << std::get<bool>(obj.m_value);
                break;
            case JSON_INT_T:
                os << std::get<int>(obj.m_value);
                break;
            case JSON_DOUBLE_T:
                os << std::get<double>(obj.m_value);
                break;
            case JSON_NULL_T:
                os << "null";
                break;
            case JSON_STRING_T:
                os << std::quoted(std::get<std::string>(obj.m_value));
                break;
            case JSON_LIST_T: {
                os << "[";
                const auto &list = std::get<json_list_t>(obj.m_value);
                size_t list_len = list.size();
                if (!list.empty()) {
                    os << list[0];
                    for (size_t i = 1; i < list_len; i++) {
                        os << ", " << list[i];
                    }
                }
                os << "]";
                break;
            }
            case JSON_DICT_T: {
                os << "{";
                const auto &dict = std::get<json_dict_t>(obj.m_value);
                if (!dict.empty()) {
                    auto iter = dict.begin();
                    std::cout << '"' << iter->first << "\": " << iter->second;
                    iter++;
                    for (; iter != dict.end(); iter++) {
                        std::cout << ",\"" << iter->first << "\": " << iter->second;
                    }
                }
                os << "}";
                break;
            }
            default:
                break;
        }
        return os;
    }

    std::pair<JSONObject, size_t> JSONObject::try_parse_null(std::string_view json) {
        std::string_view alpha = read_alpha(json);
        if (alpha == "null") {
            return {JSONObject{nullptr}, 4};
        }
        throw JSONParseException("Missing value for key");
    }

    std::pair<JSONObject, size_t> JSONObject::try_parse_bool(std::string_view json) {
        std::string_view alpha = read_alpha(json);
        if (alpha != "true" && alpha != "false") {
            throw JSONParseException("Missing value for key");
        }
        return {JSONObject{alpha == "true"}, alpha.length()};
    }

    std::string_view JSONObject::read_alpha(std::string_view json) {
        const char *str = json.data();
        while (std::isalpha(*str)) {
            str++;
        }
        size_t len = str - json.data();
        std::string_view alpha(json.data(), len);
        return alpha;
    }

    std::pair<JSONObject, size_t> JSONObject::try_parse_list(std::string_view json) {
        assert(json[0] == '[');
        char begin;
        std::vector<JSONObject> list;
        size_t total_eaten{};
        while ((begin = json[0]) != '\0' && begin != ']') {
            json = std::string_view(json.data() + 1);
            auto [obj, eaten] = JSONObject::inner_parse(json);
//            std::cout << "list item: " << obj << std::endl;
            json = std::string_view(json.data() + eaten);
            total_eaten += eaten + 1;

            begin = json[0];
            if (begin != ',' && begin != ']') {
                throw JSONParseException("Unexpected character '" + std::string(1, begin) + "'");
            }
            list.push_back(obj);
        }
        if (begin != ']') {
            throw JSONParseException("Missing bracket ']'");
        }

        // eat ']'
        total_eaten++;
        return {JSONObject{std::move(list)}, total_eaten};
    }

    std::pair<JSONObject, size_t> JSONObject::try_parse_string(std::string_view json) {
        auto [str, eaten] = _try_parse_string(json);
        return {JSONObject{std::move(str)}, eaten};
    }

    std::pair<std::string, size_t> JSONObject::_try_parse_string(std::string_view json) {
        enum {
            Raw,
            Escaped
        } phase = Raw;

        assert(json[0] == '"');
        const char *str = json.data() + 1;
        char c;
        std::string buf;
        while ((c = *str) != '\0') {
            if (phase == Raw) {
                if (c == '\\') {
                    phase = Escaped;
                } else if (c == '"') {
                    break;
                } else {
                    buf += c;
                }
            } else {
                buf += unescaped_char(c);
                phase = Raw;
            }
//            std::cout << "buf: " << buf << std::endl;
            str++;
        }
        if (c == '\0') {
            throw JSONParseException("EOF: No close string '\"' found.");
        }
        return {buf, (str - json.data()) + 1};
    }

    std::pair<JSONObject, size_t> JSONObject::try_parse_dict(std::string_view json) {
        assert(json[0] == '{');
        // eat '{' first;
        size_t total_eaten{};
        char c;
        json_dict_t dict{};
        std::string key;
        JSONObject value;
        size_t eaten;
        size_t eaten_blanks;
        while ((c = json[0]) != '\0' && c != '}') {
            // parse key
            eaten_blanks = count_blanks(json.data() + 1);
            json = std::string_view(json.data() + 1 + eaten_blanks);
            total_eaten += 1 + eaten_blanks;

            if (json[0] != '"') {
                throw JSONParseException("Key must be quoted");
            }

            std::tie(key, eaten) = _try_parse_string(json);
            total_eaten += eaten;

            if ((c = json[eaten]) != ':') {
                throw JSONParseException("Unexpected Character '" + std::string(1, c) + "', expecting a semicolon.");
            }
            // eat ':'
            total_eaten++;

            // parse value
            json = std::string_view(json.data() + eaten + 1);
            std::tie(value, eaten) = inner_parse(json);
            total_eaten += eaten;

            if ((c = json[eaten]) != ',' && c != '}') {
                throw JSONParseException("Unexpected character '" + std::string(1, c) + "'");
            }

//            std::cout << "put " << key << " -> " << value << std::endl;

            dict[std::move(key)] = std::move(value);
            json = std::string_view(json.data() + eaten);
        }

        // eat '}'
        total_eaten++;

        return {JSONObject{dict}, total_eaten};
    }

    JSONObject &JSONObject::operator[](const std::string &key) {
        if (type() == JSON_DICT_T) {
            return std::get<json_dict_t>(m_value)[key];
        }

        throw std::runtime_error("Invalid operation: The JSON object is not a dictionary.");
    }

    std::unordered_set<std::string> JSONObject::keySet() {
        std::unordered_set<std::string> set;
        if (type() == JSON_DICT_T) {
            const auto &dict = std::get<json_dict_t>(m_value);
            for (const auto &[key, obj]: dict) {
                set.emplace(key);
            }
        }
        return set;
    }

    JSONObject &JSONObject::operator[](size_t index) {
        if (type() == JSON_LIST_T) {
            return std::get<json_list_t>(m_value).at(index);
        }

        throw std::runtime_error("Invalid operation: The JSON object is not a list.");
    }

    const char *JSONParseException::what() const noexcept {
        return reason.data();
    }

    JSONParseException::JSONParseException(std::string &&reason) : reason(reason) {}
}