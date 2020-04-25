#include "vdb_response.h"

vdb::Response::Response(vdb::Row *row, uint16_t row_count)
{
	for (size_t i = 0; i < row_count; ++i)
	{
		rows.push_back(row[i]);
	}
}

vdb::Row &vdb::Response::operator[](const size_t index)
{
	return rows[index];
}

vdb::Row &vdb::Response::at(const size_t index)
{
	return rows.at(index);
}

uint16_t vdb::Response::size()
{
	return rows.size();
}

