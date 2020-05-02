#ifndef VDB_RESPONSE_H_
#define VDB_RESPONSE_H_

#include <cstdint>
#include <vector>
#include "vdb_row.h"

namespace vdb
{

class Response
{
private:
	std::vector<Row> rows;
public:
	Response() = default;
	Response(Row *row, uint16_t row_count)
	{
		for (size_t i = 0; i < row_count; ++i)
		{
			rows.push_back(row[i]);
		}
	}

	Response &push_back(Row &row)
	{
		rows.push_back(row);
		return *this;
	}
	Row &operator[](const size_t index)
	{
		return rows[index];
	}
	Row &at(const size_t index)
	{
		return rows.at(index);
	}

	uint16_t size()
	{
		return rows.size();
	}
};

}

#endif // !VDB_RESPONSE_H_
