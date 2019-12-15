#ifndef VDB_RESPONSE_H_
#define VDB_RESPONSE_H_

#include "vdb_row.h"
#include <cstring>
#include <cstdint>

namespace vdb
{
class Response
{
private:
	Row *row = nullptr;
	uint16_t row_count;
public:
	Response();
	Response(Row *row_, uint16_t row_count_);
	Response(const Response &resp);
	Response &operator=(const Response &resp);
	Row &operator[](const size_t index);
	uint16_t size();
	~Response();
};
}



#endif // !VDB_RESPONSE_H_
