#ifndef Value_H
#define Value_H

#include <string>
#include <cstdint>
#include <cstring>
#include <type_traits>


namespace vdb
{
class Value
{
private:
    char *buff = nullptr;
    uint8_t type;
    int int_val() const;
    double double_val() const;
public:
    Value(const Value &val);
    void operator=(Value &val);
    Value();
    Value(int val);
    Value(double val);
    Value(char val);
    Value(char *val, int size);
    Value(const char *val);
    operator int() const;
    operator double() const;
    operator char() const;
    operator char *() const;
    std::string to_string();
    ~Value();
};
}
#endif // !Value_H
