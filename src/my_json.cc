#include "my_json.h"
#include <assert.h>
#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <exception>
#include <map>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>
namespace json {

void Trim(std::string &s) {
    const char *temp = s.c_str();
    size_t begin = 0;
    while (begin < s.size() && isspace(temp[begin])) {
        begin++;
    }
    size_t end = s.size() - 1;
    while (end >= 0 && isspace(temp[end])) {
        end--;
    }
    s = s.substr(begin, end - begin + 1);
}

size_t FindEndOfString(const std::string &s) {
    assert(s[0] == '\"' && "s must start with '\"'");
    size_t index = 1;
    while (true) {
        size_t pos = s.find("\"", index);
        if (pos == std::string::npos) {
            return std::string::npos;
        }
        if (pos - 1 >= 0 && s[pos - 1] == '\\') {
            index = pos + 1;
            continue;
        }
        return pos;
    }
}

size_t FindEndOfObject(const std::string &s) {
    assert(s[0] == '{' && "s must start with '{");
    int count = 1;
    for (size_t i = 1; i < s.size();) {
        if (s[i] == '\"') {
            // 忽略字符串之间的符号
            size_t s_end = FindEndOfString(s.substr(i));
            if (s_end == std::string::npos) {
                return s_end;
            }
            i += s_end + 1;
            continue;
        }
        if (s[i] == '{') {
            ++count;
        } else if (s[i] == '}') {
            --count;
        }
        if (count == 0) {
            return i;
        }
        ++i;
    }
    return std::string::npos;
}
size_t FindEndOfArray(const std::string &s) {
    assert(s[0] == '[' && "s must start with '['");
    int count = 1;
    for (size_t i = 1; i < s.size();) {
        if (s[i] == '\"') {
            // 忽略字符串之间的符号
            size_t s_end = FindEndOfString(s.substr(i));
            if (s_end == std::string::npos) {
                return s_end;
            }
            i += s_end + 1;
            continue;
        }
        if (s[i] == '[') {
            ++count;
        } else if (s[i] == ']') {
            --count;
        }
        if (count == 0) {
            return i;
        }
        ++i;
    }
    return std::string::npos;
}
Json::Json() : m_type(JSON_NULL) {}

Json::Json(bool value) : m_type(JSON_BOOL) { m_value.m_bool = value; }
Json::Json(int value) : m_type(JSON_INT) { m_value.m_int = value; }
Json::Json(double value) : m_type(JSON_DOUBLE) { m_value.m_double = value; }
Json::Json(const char *value) : m_type(JSON_STRING) {
    m_value.m_string = new std::string(value);
}
Json::Json(const std::string &value) : m_type(JSON_STRING) {
    m_value.m_string = new std::string(value);
}
Json::Json(const Json &other) { copy(other); }
void Json::copy(const Json &other) {
    m_type = other.m_type;
    switch (m_type) {
    case JSON_NULL:
        break;
    case JSON_BOOL:
        m_value.m_bool = other.m_value.m_bool;
        break;
    case JSON_INT:
        m_value.m_int = other.m_value.m_int;
        break;
    case JSON_DOUBLE:
        m_value.m_double = other.m_value.m_double;
        break;
    case JSON_STRING:
        m_value.m_string = new std::string(*(other.m_value.m_string));
        break;
    case JSON_ARRAY:
        m_value.m_array = new std::vector<Json>(*other.m_value.m_array);
        break;
    case JSON_OBJECT:
        m_value.m_object =
            new std::map<std::string, Json>(*other.m_value.m_object);
        break;
    }
}
Json::Json(Type type) : m_type(type) {
    switch (m_type) {
    case JSON_NULL:
        break;
    case JSON_BOOL:
        m_value.m_bool = false;
        break;
    case JSON_INT:
        m_value.m_int = 0;
        break;
    case JSON_DOUBLE:
        m_value.m_double = 0.0;
        break;
    case JSON_STRING:
        m_value.m_string = new std::string("");
        break;
    case JSON_ARRAY:
        m_value.m_array = new std::vector<Json>();
        break;
    case JSON_OBJECT:
        m_value.m_object = new std::map<std::string, Json>();
        break;
    }
}
void Json::clear() {
    switch (m_type) {
    case JSON_STRING:
        delete m_value.m_string;
        break;
    case JSON_ARRAY:
        delete m_value.m_array;
        break;
    case JSON_OBJECT:
        delete m_value.m_object;
        break;
    default:
        break;
    }
    m_type = JSON_NULL;
}
Json::~Json() { clear(); }

Json::operator bool() {
    if (m_type == JSON_BOOL) {
        return m_value.m_bool;
    }
    throw std::logic_error("type error, not bool value");
}
Json::operator int() {
    if (m_type == JSON_INT) {
        return m_value.m_int;
    }
    throw std::logic_error("type error, not int value");
}
Json::operator double() {
    if (m_type == JSON_DOUBLE) {
        return m_value.m_double;
    }
    throw std::logic_error("type error, not double value");
}
Json::operator std::string() {
    if (m_type == JSON_STRING) {
        return *m_value.m_string;
    }
    throw std::logic_error("type error, not string value");
}

Json &Json::operator[](int index) {
    if (m_type == JSON_NULL) {
        m_type = JSON_ARRAY;
        m_value.m_array = new std::vector<Json>();
    } else if (m_type != JSON_ARRAY) {
        throw std::logic_error("type error, not array or null type");
    }

    if (index < 0) {
        throw std::logic_error("index must not be negative");
    }

    auto array = m_value.m_array;

    if ((size_t)index >= array->size()) {
        for (size_t i = array->size(); i <= (size_t)index; ++i) {
            array->push_back(Json());
        }
    }
    return array->at(index);
}

bool Json::operator==(const Json &other) const {
    if (m_type != other.m_type) {
        return false;
    }
    switch (m_type) {
    case JSON_ARRAY:
        return *m_value.m_array == *(other.m_value.m_array);
    case JSON_OBJECT:
        return *m_value.m_object == *(other.m_value.m_object);
    case JSON_STRING:
        return *m_value.m_object == *(other.m_value.m_object);
    default:
        return memcmp(&m_value.m_int, &m_value.m_int, sizeof(Value)) == 0;
    }
}
bool Json::operator!=(const Json &other) const { return !(*this == other); }

void Json::append(const Json &other) {
    if (m_type == JSON_NULL) {
        m_type = JSON_ARRAY;
        m_value.m_array = new std::vector<Json>();
    } else if (m_type != JSON_ARRAY) {
        throw std::logic_error("type error, not array or null type");
    }

    m_value.m_array->push_back(other);
}
std::string Json::str() const {
    std::stringstream ss;
    switch (m_type) {
    case JSON_NULL:
        ss << "";
        break;
    case JSON_BOOL:
        ss << (m_value.m_bool ? "true" : "false");
        break;
    case JSON_INT:
        ss << m_value.m_int;
        break;
    case JSON_DOUBLE:
        ss << m_value.m_double;
        break;
    case JSON_STRING:
        ss << "\"";
        ss << *m_value.m_string;
        ss << "\"";
        break;
    case JSON_ARRAY:
        ss << "[";
        for (auto it = (*m_value.m_array).begin();
             it != (*m_value.m_array).end();) {
            ss << it->str();
            bool f = it->str().empty();
            ++it;
            if (it != (*m_value.m_array).end() && !f) {
                ss << ",";
            }
        }
        ss << "]";
        break;
    case JSON_OBJECT:
        ss << "{";
        for (auto it = (*m_value.m_object).begin();
             it != (*m_value.m_object).end(); ++it) {
            if (it != (*m_value.m_object).begin()) {
                ss << ",";
            }
            ss << "\"";
            ss << it->first;
            ss << "\":";
            ss << it->second.str();
        }
        ss << "}";
        break;
    }
    return ss.str();
}
Json &Json::operator[](const char *key) {
    if (m_type == JSON_NULL) {
        m_type = JSON_OBJECT;
        m_value.m_object = new std::map<std::string, Json>();
    } else if (m_type != JSON_OBJECT) {
        throw std::logic_error("type error, type must be null or object");
    }
    return (*m_value.m_object)[key];
}
void Json::operator=(const Json &other) {
    clear();
    copy(other);
}

std::vector<Json>::iterator Json::begin() {
    if (m_type != JSON_ARRAY) {
        throw std::logic_error("type error, not array");
    }
    return (*m_value.m_array).begin();
}
std::vector<Json>::iterator Json::end() {
    if (m_type != JSON_ARRAY) {
        throw std::logic_error("type error, not array");
    }
    return (*m_value.m_array).end();
}
bool Json::asBool() {
    if (m_type != JSON_BOOL) {
        throw std::logic_error("type error, not bool");
    }
    return m_value.m_bool;
}
int Json::asInt() {
    if (m_type != JSON_INT) {
        throw std::logic_error("type error, not int");
    }
    return m_value.m_int;
}
double Json::asDouble() {
    if (m_type != JSON_DOUBLE) {
        throw std::logic_error("type error, not double");
    }
    return m_value.m_double;
}
std::vector<Json> &Json::asArray() {
    if (m_type != JSON_ARRAY) {
        throw std::logic_error("type error, not array");
    }
    return *m_value.m_array;
}
std::map<std::string, Json> &Json::asObject() {
    if (m_type != JSON_OBJECT) {
        throw std::logic_error("type error, not object");
    }
    return *m_value.m_object;
}
bool Json::has(int index) {
    if (m_type != JSON_ARRAY) {
        throw std::logic_error("type error, not array");
    }
    return index >= 0 && (size_t)index < m_value.m_array->size();
}
bool Json::has(const char *key) {
    if (m_type != JSON_OBJECT) {
        throw std::logic_error("type error, not object");
    }
    auto it = (*m_value.m_object).find(key);
    return it != (*m_value.m_object).end();
}

void Json::remove(int index) {
    if (m_type != JSON_ARRAY) {
        throw std::logic_error("type error, not array");
    }
    auto array = *m_value.m_array;
    if (index < 0 || (size_t)index >= array.size()) {
        throw std::logic_error("index out of range");
    }
    array.erase(array.begin() + index);
}
void Json::remove(const char *key) {
    if (!has(key)) {
        throw std::logic_error("key is not exist");
    }
    auto object = m_value.m_object;
    object->erase(key);
}

Json GetObjectFromString(std::string &s, bool &error_flag) {
    size_t opos = FindEndOfObject(s);
    if (opos == std::string::npos) {
        error_flag = true;
        return Json();
    }
    size_t pos = s.find(',', opos + 1);
    std::string temp;
    if (pos == std::string::npos) {
        temp = s;
        pos = s.size() - 1;
    } else {
        temp = s.substr(0, pos);
    }
    Json value = Json::ParseFromString(temp);
    s = s.substr(pos + 1);
    return value;
}
Json GetArrayFromString(std::string &s, bool &error_flag) {
    size_t opos = FindEndOfArray(s);
    if (opos == std::string::npos) {
        error_flag = true;
        return Json();
    }
    size_t pos = s.find(',', opos + 1);
    std::string temp;
    if (pos == std::string::npos) {
        temp = s;
        pos = s.size() - 1;
    } else {
        temp = s.substr(0, pos);
    }
    Json value = Json::ParseFromString(temp);
    s = s.substr(pos + 1);
    return value;
}
Json GetStringFromString(std::string &s, bool &error_flag) {
    size_t opos = FindEndOfString(s);
    if (opos == std::string::npos) {
        error_flag = true;
        return Json();
    }
    size_t pos = s.find(',', opos + 1);
    std::string temp;
    if (pos == std::string::npos) {
        temp = s;
        pos = s.size() - 1;
    } else {
        temp = s.substr(0, pos);
    }
    Json value = Json::ParseFromString(temp);
    s = s.substr(pos + 1);
    return value;
}

Json Json::ParseFromString(std::string &s) {
    bool error_flag = false;
    Trim(s);
    if (s.empty()) {
        return Json();
    }

    if (s[0] == 'f') {
        // bool:false
        if (s == "false") {
            return Json(false);
        }
        error_flag = true;
    } else if (s[0] == 't') {
        // bool:true
        if (s == "true") {
            return Json(true);
        }
        error_flag = true;
    } else if ((s[0] >= '0' && s[0] <= '9') || s[0] == '+' || s[0] == '-') {
        // number: int or double
        size_t pos = s.find('.');
        if (pos == std::string::npos) {
            // int
            char *endptr = nullptr;
            long v = strtol(s.c_str(), &endptr, 0);
            if (!(*endptr)) {
                if (v > INT32_MAX || v < INT32_MIN) {
                    throw std::logic_error("the number out of int32_t's range");
                }
                return Json((int)v);
            }
            error_flag = true;
        } else {
            // double
            char *endptr = nullptr;
            double v = strtod(s.c_str(), &endptr);
            if (!(*endptr)) {
                return Json(v);
            }
            error_flag = true;
        }
    } else if (s[0] == '\"') {
        // string: "....."， 这之间不能有其他的"，但是可以有\"
        size_t pos = FindEndOfString(s);
        if (pos == std::string::npos || pos != s.size() - 1) {
            error_flag = true;
        } else {
            return Json(s.substr(1, s.size() - 2));
        }
    } else if (s[0] == '[') {
        // array: [true, 1, false, "haha"];
        if (s[s.size() - 1] != ']') {
            error_flag = true;
        } else {
            Json result(JSON_ARRAY);
            s = s.substr(1, s.size() - 2);
            while (!s.empty()) {
                Trim(s);
                if (s[0] == '\"') {
                    Json value = GetStringFromString(s, error_flag);
                    if (error_flag) {
                        break;
                    }
                    result.append(value);
                } else if (s[0] == '{') {
                    Json value = GetObjectFromString(s, error_flag);
                    if (error_flag) {
                        break;
                    }
                    result.append(value);
                } else if (s[0] == '[') {
                    Json value = GetArrayFromString(s, error_flag);
                    if (error_flag) {
                        break;
                    }
                    result.append(value);
                } else {
                    // next is not string
                    size_t pos = s.find_first_of(',');
                    if (pos == std::string::npos) {
                        pos = s.size() - 1;
                    }
                    std::string temp = s.substr(0, pos);
                    result.append(ParseFromString(temp));
                    s = s.substr(pos + 1);
                }
            }
            if (!error_flag)
                return result;
        }
    } else if (s[0] == '{') {
        // object:{"key1": 12, "key2" : object}
        if (s[s.size() - 1] != '}') {
            error_flag = true;
        } else {
            Json result(JSON_OBJECT);
            s = s.substr(1, s.size() - 2);
            while (!s.empty()) {
                Trim(s);
                if (s[0] != '\"') {
                    error_flag = true;
                    break;
                }
                size_t key_pos = FindEndOfString(s);
                if (key_pos == std::string::npos) {
                    error_flag = true;
                    break;
                }
                std::string key = s.substr(1, key_pos - 1);
                size_t spos = s.find(':', key_pos + 1);
                if (spos == std::string::npos) {
                    error_flag = true;
                }
                s = s.substr(spos + 1);
                Trim(s);
                // 检查value的值类型
                if (s[0] == '\"') {
                    // value=string
                    Json value = GetStringFromString(s, error_flag);
                    if (error_flag) {
                        break;
                    }
                    result[key.c_str()] = value;
                } else if (s[0] == '{') {
                    // value=object
                    Json value = GetObjectFromString(s, error_flag);
                    if (error_flag) {
                        break;
                    }
                    result[key.c_str()] = value;
                } else if (s[0] == '[') {
                    Json value = GetArrayFromString(s, error_flag);
                    if (error_flag) {
                        break;
                    }
                    result[key.c_str()] = value;
                } else {
                    size_t pos = s.find(',');
                    std::string temp;
                    if (pos == std::string::npos) {
                        temp = s;
                        s = "";
                    } else {
                        temp = s.substr(0, pos);
                        s = s.substr(pos + 1);
                    }
                    Json value = ParseFromString(temp);
                    result[key.c_str()] = value;
                }
            }
            if (!error_flag)
                return result;
        }
    }

    if (error_flag) {
        throw std::logic_error("json string format error");
    }
    return Json();
}

} // namespace json

// namespace json