#include "json.h"
#include <fstream>

int main() {
    std::ifstream is("../test/succeed.json");
    std::string str((std::istreambuf_iterator<char>(is)),
                    std::istreambuf_iterator<char>());
    json::JSONObject obj1 = json::JSONObject::parse(str);
    std::cout << obj1["list"][2]["hello"] << std::endl;
    std::cout << obj1.stringify(4) << std::endl;
    std::cout << obj1.stringify(2) << std::endl;
    json::JSONObject obj2 = json::JSONObject::parse("[false]");
    std::cout << obj2 << std::endl;
//    json::JSONObject obj3 = json::JSONObject::parse(" true");
//    std::cout << obj3 << std::endl;
//    json::JSONObject obj4 = json::JSONObject::parse("tx");
//    std::cout << obj4 << std::endl;
}