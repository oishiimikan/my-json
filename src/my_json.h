#ifndef __MY_JSON_H_
#define __MY_JSON_H_

#include <map>
#include <string>
#include <vector>
namespace json {

/*
    去除字符串前后的空字符
*/
void Trim(std::string &s);
/*
    找到一个字符串的结尾，即"，并不是\"；
    然后将它的位置返回
*/
size_t FindEndOfString(const std::string &s);
/*
    找到一个对象的结尾，该字符串一定要以'{'开始，返回结束位置的下标
*/
size_t FindEndOfObject(const std::string &s);


size_t FindEndOfArray(const std::string &s);

class Json {
  public:
    enum Type {
        JSON_NULL = 0,
        JSON_BOOL,
        JSON_INT,
        JSON_DOUBLE,
        JSON_STRING,
        JSON_ARRAY,
        JSON_OBJECT
    };

    Json();

    ~Json();

    Json(bool value);
    Json(int value);
    Json(double value);
    Json(const char *value);
    Json(const std::string &value);
    Json(const Json &other);
    Json(Type type);

    operator bool();
    operator int();
    operator double();
    operator std::string();

    Json &operator[](int index);
    Json &operator[](const char *key);

    void operator=(const Json &other);
    void append(const Json &other);


    bool operator==(const Json& other) const;
    bool operator!=(const Json& other) const;
    std::string str() const;

    std::vector<Json>::iterator begin();
    std::vector<Json>::iterator end();
    bool asBool();
    int asInt();
    double asDouble();
    std::vector<Json> &asArray();
    std::map<std::string, Json> &asObject();

    bool isBool() const { return m_type == JSON_BOOL; }
    bool isNull() const { return m_type == JSON_NULL; }
    bool isInt() const { return m_type == JSON_INT; }
    bool isDouble() const { return m_type == JSON_DOUBLE; }
    bool isArray() const { return m_type == JSON_ARRAY; }
    bool isObject() const { return m_type == JSON_OBJECT; }

    bool has(int index);
    bool has(const char *key);

    void remove(int index);
    void remove(const char *key);

  public:
    static Json ParseFromString(std::string &s);

  private:
    void copy(const Json &other);

    void clear();

  private:
    union Value {
        bool m_bool;
        int m_int;
        double m_double;
        std::string *m_string;
        std::vector<Json> *m_array;
        std::map<std::string, Json> *m_object;
    };

    Type m_type;
    Value m_value;
};
} // namespace json
#endif
