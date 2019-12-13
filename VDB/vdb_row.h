#ifndef ROW_H
#define ROW_H

#include <cstdint>
#include "vdb_value.h"
#include <stdexcept>

namespace vdb
{
class Row
{
private:
    Value *values = nullptr;
	size_t pointer = 0; // points to the current array index (needed for push_back())
    size_t size = 0;
    
    template <typename T>
    void count_size(T val)        
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
    void get_value(T val)         
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
	Row();
    Row(size_t size_);

    template <typename... Args>
	Row(Args... args)
	{
		count_size(args...);
		values = new vdb::Value[size];
		get_value(args...);
	}

    Row(const Row &row);
    Row &operator=(const Row &row);

    template <typename T>
    Row &push_back(T val) // doesn't work
	{
		if (pointer < size)
		{
			vdb::Value value(val);
			values[pointer++] = value; 
		}
		return *this;
	}

	Value &operator[](const uint8_t index);
	void resize(size_t size_);
    size_t get_size();
    ~Row();
};

}
#endif