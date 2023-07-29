#ifndef JSON_PARSER_JSON_H
#define JSON_PARSER_JSON_H

#include <variant>
#include <vector>
#include <string>
#include <iostream>
#include <unordered_map>

namespace json {
    class JSONObject;

    enum JSONType {
        JSON_NULL_T,
        JSON_BOOL_T,
        JSON_INT_T,
        JSON_DOUBLE_T,
        JSON_STRING_T,
        JSON_LIST_T,
        JSON_DICT_T
    };
    using json_null_t = std::nullptr_t;
    using json_list_t = std::vector<JSONObject>;
    using json_dict_t = std::unordered_map<std::string, JSONObject>;
    using value_t = std::variant<
            json_null_t,
            bool,
            int,
            double,
            std::string,
            json_list_t,
            json_dict_t
    >;

    class JSONObject {
    private:
        value_t m_value;

    public:
        JSONObject();

        explicit JSONObject(value_t &&v);

        static JSONObject parse(std::string_view json);

    private:
        static void skip_blank(std::string_view &json);

        static JSONObject try_parse_number(std::string_view &json);

        friend std::ostream &operator<<(std::ostream &os, const JSONObject &obj);
    };

    class JSONParseException : public std::exception {
    private:
        std::string reason;
    public:
        explicit JSONParseException(std::string &&reason);

        [[nodiscard]] const char *what() const noexcept override;
    };
}

#endif //JSON_PARSER_JSON_H
