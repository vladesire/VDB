#ifndef Value_H
#define Value_H

#include <string>
#include <cstdint>
#include <cstring>
#include <type_traits>


namespace vdb
{/*
    class ValueBase
    {

    public:
        template<typename T>
        operator T() 
        {
            return dynamic_cast<typename vdb::Value<T> *>(this);
        };

    };

    template <class VAL>
    class Value : public ValueBase
    {
    private:
        VAL value;
        static_assert(std::is_same<VAL, int>::value || std::is_same<VAL, double>::value || std::is_same<VAL, char>::value, "This type is forbidden!");
    public:
        Value() { }
        Value(VAL val) { value = val; }
        operator VAL() const { return value; }
        VAL &operator=(VAL val)
        {
            value = val;
            return value;
        }
        ~Value() { }
    };

    template <>
    class Value<char *> : public ValueBase
    {
    private:
        char *value;
        bool is_allocated;
    public:
        ~Value()
        {
            if (is_allocated)
                delete[] value;
        }
        Value(char *val)
        {
            value = new char[std::strlen(val) + 1];
            is_allocated = true;
            std::strcpy(value, val);
        }
        Value() { value = nullptr; is_allocated = false; }
        char * operator=(char * val)
        {
            if (is_allocated)
                delete[] value;

            value = new char[std::strlen(val) + 1];
            std::strcpy(value, val);
            return value;
        }
        operator char *() const
        {
            return value;
        }
    };
    */

    class Value
    {
    private:
        char *buff = nullptr;
        uint8_t type;
        int int_val() const
        {
            int a;
            memcpy((char *)&a, buff, 4);
            return a;
        }
        double double_val() const
        {
            double a;
            memcpy((char *)&a, buff, 8);
            return a;
        }
    public:
        Value(const Value &p2)
        {
            type = p2.type;
            int size = (type == 0) ? 4 : (type == 1 ? 8 : (type == 2 ? 1 : type == 3 ? 32 : 64));
            buff = new char[size];
            memcpy(buff, p2.buff, size);
        };
        void operator=(Value other)
        {
            type = other.type;
            int size = (type == 0) ? 4 : (type == 1 ? 8 : (type == 2 ? 1 : type == 3 ? 32 : 64));
            buff = new char[size];
            memcpy(buff, other.buff, size);
        };

        Value(int val)
        {
            type = 0;
            buff = new char[4];
            for (int i = 0; i < 4; i++)
                buff[i] = ((char *)(&val))[i];
        }
        Value(double val)
        {
            type = 1;
            buff = new char[8];
            for (int i = 0; i < 8; i++)
                buff[i] = ((char *)(&val))[i];
        }
        Value(char val)
        {
            type = 2;
            buff = new char[1];
            *buff = val;
        }
        Value(char *val, int size)
        {
            type = (size == 32) ? 3 : 4;
            buff = new char[size];
            memcpy(buff, val, size);
        }
        operator int() const
        {
            return int_val();
        }
        operator double() const
        {
            return double_val();
        }

        operator char() const
        {
            return buff[0];
        }
        operator char *() const
        {
            return buff;
        }
        std::string to_string()
        {
            switch (type)
            {
                case 0:
                    return std::to_string(int_val());
                case 1:
                    return std::to_string(double_val());
                case 2:
                    return std::string(1, buff[0]);
                case 3:
                case 4:
                    return std::string(buff);
            }
            return std::string("");
        }
        ~Value()
        {
            delete[] buff;
        }
    };
}
#endif // !Value_H
