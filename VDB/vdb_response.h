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
    Response() {}

    Response(Row *row_, size_t row_count_)
    { 
		row_count = row_count_;

		row = new Row[row_count];
		
		for (size_t i = 0; i < row_count; ++i)
			row[i] = row_[i];

    }
	Response(const Response &resp)
	{
		row_count = resp.row_count;
		delete[] row;

		row = new Row[row_count];

		for (int i = 0; i < row_count; ++i)
		{
			row[i] = resp.row[i];
		}
	}
	Response &operator=(const Response &resp)
	{
		if (this == &resp)
			return *this;

		row_count = resp.row_count;
		delete[] row;

		row = new Row[row_count];

		for (int i = 0; i < row_count; ++i)
		{
			row[i] = resp.row[i];
		}

		return *this;
	}
    // Maybe I should make it const?
    Row &operator[](const size_t index)
    {
		if (index < row_count)
			return row[index];
		else
			throw std::invalid_argument("Vse perepleteno");
    }

    size_t size()
    {
        return row_count;
    }

    ~Response()
    {
		delete[] row;
    }

};
}



#endif // !VDB_RESPONSE_H_
