#ifndef ROW_H
#define ROW_H

#include <cstdint>
#include "vdb_value.h"
#include "meta.h"

namespace vdb
{
class Row
{
private:
    Value *values = nullptr;
    bool is_allocated = false;
    size_t pointer = 0; // points to the current array index (needed for push_back())
    size_t size = 0;
    

    template <typename T>
    void count_size(T val)         // IN DEVELOPMENT. GOOGLE variadic templates
    {
        ++size;
        return;
    }
    template <typename T, typename... Rest>
    void count_size(T val, Rest... rest)
    {
        ++size;
        count_size(rest...);
        return;
    }

    template <typename T>
    void get_value(T val)         // IN DEVELOPMENT. GOOGLE variadic templates
    {
        push_back(val);
        return;
    }
    template <typename T, typename... Rest>
    void get_value(T val, Rest... rest)
    {
        push_back(val);
        get_value(rest...);
        return;
    }
public:
    Row(vdb::meta *m) : size(m->colcount)
    {
        values = new Value[size];
    }
    template <typename... Args>
    Row(Args... args)
    {
        count_size(args...);
        values = new Value[size];
        get_value(args...);
    }
    Row(const Row &row)
    {
        size = row.size;
        pointer = row.pointer;
        if (row.is_allocated)
        {
            values = new Value[size];
            is_allocated = true;
            memcpy(values, row.values, size);
        }
    }
    void operator=(Row &row)
    {
        size = row.size;
        pointer = row.pointer;
        if (is_allocated)
            delete[] values;
        if (row.is_allocated)
        {
            values = new Value[size];
            is_allocated = true;
            memcpy(values, row.values, size);
        }
    }
    template <typename T>
    Row *push_back(T val)
    {
        if (pointer < size)
        {
            Value value(val);
            values[pointer++] = value;
            return this;
        }
        else
            return nullptr;
    }
    Value &operator[] (size_t index)
    {
        if (index < size)
            return values[index];
        else
            throw std::invalid_argument("Nu chto skazat, ya vizhu kto-to nastupil na grabli!");
    }
    size_t get_size()
    {
        return size;
    }
    ~Row()
    {
        if (is_allocated)
            delete[] values;
    }
};
    

/*class Row_
{
private:
    vdb::ValueBase *row = nullptr;
    uint8_t max, index = 0;
public:
    Row(meta *db)
    {
        max = db->colcount;
        row = new ValueBase[max];
    }
    ~Row()
    {
        delete[] row;
    }
    template <typename T>
    void push_back(vdb::Value<T> val)
    {
        if (index >= max)
            return;
        row[index++] = val;
    }

    ValueBase &operator[] (uint8_t i)
    {
        if (i >= max)
            throw std::invalid_argument("Index is out of bounds!");
        return row[i];
    }

};*/
}
#endif