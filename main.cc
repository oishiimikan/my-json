#include "src/my_json.h"
#include <exception>
#include <iostream>
#include <map>
#include <string>
#include <utility>
using namespace json;
int main() {
    std::string s = R"(
{
    "employee": {
        "firstName": "Alice",
        "lastName": "Johnson",
        "position": "Software Engineer",
        "salary": 80000
    },
    "projects": [
        {
            "name": "Project A",
            "status": "In Progress",
            "team": ["Alice", "Bob", "Charlie"]
        },
        {
            "name": "Project B",
            "status": "Completed",
            "team": ["David", "Eve"]
        }
    ]
}
)";

    Json value = Json::ParseFromString(s);
    std::cout << value.str() << std::endl;
    if (value.isObject()) {
        std::map<std::string, Json> object = value.asObject();
        for(auto it : object){
            std::cout << "key:" << it.first << std::endl;
            std::cout << "value:" << it.second.str() << std::endl;
        }
    }
}