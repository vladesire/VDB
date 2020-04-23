#include "vdb_row.h"


vdb::Row::Row(size_t size)
{
	values.reserve(size);
}

vdb::Value &vdb::Row::operator[](const uint16_t index)
{
	return values[index];
}

vdb::Value &vdb::Row::at(const uint16_t index)
{
	if (index < values.size())
		return values[index];
	else
		throw std::exception("vdb::Row: Index is out of bound");
}
