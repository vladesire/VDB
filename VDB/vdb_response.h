#ifndef VDB_RESPONSE_H_
#define VDB_RESPONSE_H_

#include "vdb_row.h"
#include <cstdint>
#include <vector>

namespace vdb
{
class Response
{
private:
	std::vector<Row> rows;
public:
	Response() = default;
	Response(Row *row_, uint16_t row_count_);

	Response &push_back(Row &row);
	Row &operator[](const size_t index);
	Row &at(const size_t index);

	uint16_t size();
};
}



#endif // !VDB_RESPONSE_H_
