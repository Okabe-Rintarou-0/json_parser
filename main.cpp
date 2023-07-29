#include "json.h"

int main() {
    json::JSONObject obj1 = json::JSONObject::parse("123");
    std::cout << obj1 << std::endl;
    json::JSONObject obj2 = json::JSONObject::parse("123.321");
    std::cout << obj2 << std::endl;
    json::JSONObject obj3 = json::JSONObject::parse(" 123");
    std::cout << obj3 << std::endl;
    json::JSONObject obj4 = json::JSONObject::parse(" 123.321");
    std::cout << obj4 << std::endl;
}
