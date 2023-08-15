# Json类api介绍
## 保存的数据类型
Json类可以保存的数据类型如下所示：
```cpp
class Json {
union Value{
    bool m_bool;
    int m_int;
    double m_double;
    std::string* m_string;
    std::vector<Json>* array;
    std::map<std::string, Json>* object;
};
};
```
json数据类型可以是：
- `bool`
- `int`
- `double`
- `array`
- `object`
## 如何使用
```cpp
using namespace json;
int main(){
    Json b = true;
    Json i = 100;
    Json d = 20.5;
    Json v(Json::JSON_ARRAY);
    v.append(b);
    v.append(i);
    v.append(d);

    Json o(Json::JSON_OBJECT);
    o["vector"] = v;
    // 可以使用Json::str()将Json以字符串格式的数据输出
    std::cout << o.str() << std::endl;
}
```
上面这段代码的输出如下：
```
{"vector":[true,100,20.5]}
```
需要注意的是，对于一个Json类型，它所能保存的数据类型是不能改变的，当我们仅仅只是进行了`Json v;`, `v`的类型为`JSON_NULL`，还没有被定义，但是一旦我们对其进行了赋值操作或者使用一些固定的方法，那么它的类型就被固定了。比如：
```cc
Json v;
// 这时v的类型就被固定成了JSON_ARRAY，
v.append(100);
```
Json中有一些api的使用有类型限制，主要是为了方便操作ARRAY和OBJECT，只能用于ARRAY的函数如下，如果是JSON_NULL类型的Json使用这些函数，那么那个Json对象就会被定义为JSON_ARRAY：
```cc
// 迭代器，用于遍历数组
std::vector<Json>::iterator begin();
std::vector<Json>::iterator end();

Json &operator[](int index);
bool has(int index);
void remove(int index);
void append(const Json &other);
```
只能用于OBJECT的函数如下：
```cc
Json &operator[](const char *key);
bool has(const char *key);
void remove(const char *key);
```
# 解析字符串格式的json
我们给出一个json格式的字符串如下:
```json
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
```
我们可以使用`Json::ParseFromString(const std::string& s)`这个方法将一个json格式的字符串转成Json对象。如下：
```cc
Json value = Json::ParseFromString(s);
    std::cout << value.str() << std::endl;
    if (value.isObject()) {
        std::map<std::string, Json> object = value.asObject();
        for(auto it : object){
            std::cout << "key:" << it.first << std::endl;
            std::cout << "value:" << it.second.str() << std::endl;
        }
    }
```
控制台输出的结果如下：
```
{"employee":{"firstName":"Alice","lastName":"Johnson","position":"Software Engineer","salary":80000},"projects":[{"name":"Project A","status":"In Progress","team":["Alice","Bob","Charlie"]},{"name":"Project B","status":"Completed","team":["David","Eve"]}]}
key:employee
value:{"firstName":"Alice","lastName":"Johnson","position":"Software Engineer","salary":80000}
key:projects
value:[{"name":"Project A","status":"In Progress","team":["Alice","Bob","Charlie"]},{"name":"Project B","status":"Completed","team":["David","Eve"]}]
```
我们可以使用类似的方式来操作解析出的字符串结果