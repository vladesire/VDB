#pragma once
#ifndef VDB_RESPONSE_H_
#define VDB_RESPONSE_H_

#include "vdb_row.h"

namespace vdb
{
class Response
{
private:
	Row *row = nullptr;
	size_t row_count;
public:
	Response();
	Response(Row *row_, size_t row_count_);
	Response(const Response &resp);
	Response &operator=(const Response &resp);
	Response &where(std::string &condition);
	Response &where(const char *condition);
	Row &operator[](const size_t index);
	size_t size();
	~Response();
};
}



#endif // !VDB_RESPONSE_H_
