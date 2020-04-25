#include "vdb_value.h"

#include <iostream>

void vdb::swap(Value &val1, Value &val2)
{
	val1.val.swap(val2.val);
}

vdb::Value::Value(const Value &value)
{
	if (value.type() == 3)
	{
		auto size = strlen(std::get<char *>(value.val));

		val = new char[size + 1];
		memcpy(std::get<char *>(val), std::get<char *>(value.val), size + 1);
		std::get<char *>(val)[size] = '\0';
	}
	else
	{
		val = value.val;
	}
}

vdb::Value::Value(Value &&value) noexcept
{
	val = value.val;
	value.val = 0; // to prevent deallocation in char * case 
}

vdb::Value &vdb::Value::operator=(Value value) noexcept
{
	vdb::swap(*this, value);
	return *this;
}

void vdb::Value::reset()
{
	if (val.index() == 3)
	{
		delete[] std::get<char *>(val);
		val = nullptr;
	}
}

std::string vdb::Value::to_string()
{
	switch (val.index())
	{
		case 0:
			return std::to_string(get<int>(val));
		case 1:
			return std::to_string(get<double>(val));
		case 2:
			return std::to_string(get<char>(val));
		case 3:
			return std::string(get<char *>(val));
	}
}

char *vdb::Value::cptr()
{
	switch (val.index())
	{
		case 0:
			return reinterpret_cast<char *>(std::get_if<int>(&val));
		case 1:
			return reinterpret_cast<char *>(std::get_if<double>(&val));
		case 2:
			return reinterpret_cast<char *>(std::get_if<char>(&val));
		case 3:
			return std::get<char *>(val);
	}
}

std::ostream &vdb::operator<<(std::ostream &os, Value val)
{
	os << val.to_string();
	return os;
}

