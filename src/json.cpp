#include "json.h"
#include "utils.h"

namespace json {
    JSONObject JSONObject::parse(std::string_view json) {
        skip_blank(json);
        int idx = 0;
        size_t len = json.length();
        char c;
        for (; idx < len; idx++) {
            c = json[idx];
            if (std::isdigit(c)) {
                JSONObject value = try_parse_number(json);
                return value;
            }
        }
        return {};
    }

    JSONObject::JSONObject(json::value_t &&v) : m_value(v) {}

    JSONObject::JSONObject() : JSONObject(nullptr) {}

    JSONObject JSONObject::try_parse_number(std::string_view &json) {
        int idx = 0;
        int sign = 1;
        int integer_part = 0;
        int decimal_part = 0;
        int decimal_cnt = 0;
        char c;
        value_t v;

        if (json[0] == '-') {
            idx++;
            sign = -1;
        }

        while ((c = json[idx]) >= '0' && c <= '9') {
            integer_part = integer_part * 10 + c - '0';
            idx++;
        }

        if (c == ',' || c == '\0' || c == '}') {
            // it is an integer
            json = std::string_view(json.data() + idx);
            v = integer_part * sign;
            return JSONObject{std::move(v)};
        }
        if (c != '.') {
            // meet invalid character
            goto error;
        }

        idx++;
        while (std::isdigit(c = json[idx])) {
            decimal_part = decimal_part * 10 + c - '0';
            idx++;
            decimal_cnt++;
        }

        if (c != ',' && c != '\0' && c != '}') {
            goto error;
        }

        json = std::string_view(json.data() + idx);
        v = (integer_part + decimal_part / pow10(decimal_cnt)) * sign;
        return JSONObject{std::move(v)};

        error:
        std::string error_msg = "Unexpected character \" \"";
        error_msg[error_msg.length() - 2] = c;
        throw JSONParseException(std::move(error_msg));
    }

    void JSONObject::skip_blank(std::string_view &json) {
        const char *str = json.data();
        while (std::isspace(*str)) {
            str++;
        }
        json = std::string_view(json.data() + (str - json.data()));
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
            default:
                break;
        }
        return os;
    }

    const char *JSONParseException::what() const noexcept {
        return reason.data();
    }

    JSONParseException::JSONParseException(std::string &&reason) : reason(reason) {}
}