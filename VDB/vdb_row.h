#ifndef VDB_ROW_H
#define VDB_ROW_H

#include <cstdint>
#include <stdexcept>
#include <vector>

#include "vdb_value.h"

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

	Value &operator[](const uint16_t index) // unchecked
	{
		return values[index];
	}
	Value &at(const uint16_t index) // checked
	{
		if (index < values.size())
			return values[index];
		else
			throw std::exception("vdb::Row: Index is out of bound");
	}
	void reserve(uint16_t size)
	{
		values.reserve(size);
	}
	void clear()
	{
		values.clear();
	}

	size_t size()
	{
		return values.size();
	}
};

}
#endif