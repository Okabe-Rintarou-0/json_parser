#ifndef JSON_PARSER_JSON_H
#define JSON_PARSER_JSON_H

#include <variant>
#include <vector>
#include <string>
#include <iostream>
#include <unordered_map>
#include <sstream>

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

        std::string stringify(size_t indent);

        template<typename T>
        inline T as();

        inline JSONType type() const;

        JSONObject &operator[](const std::string &key);

        JSONObject &operator[](size_t index);

    private:
        static std::pair<JSONObject, size_t> inner_parse(std::string_view json);

        static size_t count_blanks(const char *json);

        static char unescaped_char(char c);

        void _stringify(std::ostringstream &buf, size_t indent, size_t depth) const;

        static std::pair<JSONObject, size_t> try_parse_number(std::string_view json);

        static std::pair<JSONObject, size_t> try_parse_null(std::string_view json);

        static std::pair<JSONObject, size_t> try_parse_bool(std::string_view json);

        static std::pair<JSONObject, size_t> try_parse_list(std::string_view json);

        static std::pair<std::string, size_t> _try_parse_string(std::string_view json);

        static std::pair<JSONObject, size_t> try_parse_string(std::string_view json);

        static std::pair<JSONObject, size_t> try_parse_dict(std::string_view json);

        static std::string_view read_alpha(std::string_view json);

        friend std::ostream &operator<<(std::ostream &os, const JSONObject &obj);
    };

    class JSONParseException : public std::exception {
    private:
        std::string reason;
    public:
        explicit JSONParseException(std::string &&reason);

        [[nodiscard]] const char *what() const noexcept override;
    };

    template<class T>
    T JSONObject::as() {
        return std::get<T>(m_value);
    }

    JSONType JSONObject::type() const {
        return JSONType(m_value.index());
    }
}

#endif //JSON_PARSER_JSON_H
