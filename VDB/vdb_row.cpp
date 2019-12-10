#include "vdb_row.h"

vdb::Row::Row() { }
vdb::Row::Row(size_t size_) : size(size_)
{
	values = new vdb::Value[size];
}
vdb::Row::Row(const vdb::Row &row)
{
	size = row.size;
	pointer = row.pointer;
	if (row.values != nullptr)
	{
		values = new vdb::Value[size];
		for (size_t i = 0; i < size; ++i)
		{
			values[i] = row.values[i];
		}
	}
}
vdb::Row &vdb::Row::operator=(const vdb::Row &row)
{
	if (this == &row)
		return *this;
	size = row.size;
	pointer = row.pointer;
	delete[] values;
	if (row.values != nullptr)
	{
		values = new vdb::Value[size];
		for (size_t i = 0; i < size; ++i)
		{
			values[i] = row.values[i];
		}
	}
	return *this;
}
vdb::Value &vdb::Row::operator[](const uint8_t index)
{
	if (index < size)
		return values[index];
	else
		throw std::invalid_argument("Nu chto skazat, ya vizhu kto-to nastupil na grabli!");
}
size_t vdb::Row::get_size()
{
	return size;
}
vdb::Row::~Row()
{
	delete[] values;
}
void vdb::Row::resize(size_t size_)
{
	size = size_;
	values = new vdb::Value[size];
}