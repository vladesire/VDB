#ifndef VDB_ROW_H
#define VDB_ROW_H


#include "vdb_value.h"
#include <cstdint>
#include <stdexcept>
#include <vector>

namespace vdb
{
class Row
{
private:
	std::vector<Value> values;

    template <typename T>
    void get_value(T val) { values.push_back(val); }

    template <typename T, typename... Rest>
    void get_value(T val, Rest... rest) { values.push_back(val); get_value(rest...); }

public:
	Row() = default;
	Row(size_t size_);

	template <typename... Args>
	Row(Args... args)
	{
		values.reserve(sizeof...(args));
		get_value(args...);
	}

	template <typename T>
	Row &push_back(T val)
	{
		values.push_back(val);
		return *this;
	}

	Value &operator[](const uint16_t index); // unchecked
	Value &at(const uint16_t index); // checked

	void reserve(uint16_t size)
	{
		values.reserve(size);
	}

	size_t size()
	{
		return values.size();
	}

};

}
#endif