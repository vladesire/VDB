#include "vdb_response.h"

vdb::Response::Response() { }
vdb::Response::Response(vdb::Row *row_, size_t row_count_)
{
	row_count = row_count_;

	row = new vdb::Row[row_count];

	for (size_t i = 0; i < row_count; ++i)
		row[i] = row_[i];
}
vdb::Response::Response(const vdb::Response &resp)
{
	row_count = resp.row_count;
	delete[] row;

	row = new vdb::Row[row_count];

	for (int i = 0; i < row_count; ++i)
	{
		row[i] = resp.row[i];
	}
}
vdb::Response &vdb::Response::operator=(const vdb::Response &resp)
{
	if (this == &resp)
		return *this;

	row_count = resp.row_count;
	delete[] row;

	row = new vdb::Row[row_count];

	for (int i = 0; i < row_count; ++i)
	{
		row[i] = resp.row[i];
	}

	return *this;
}
vdb::Row &vdb::Response::operator[](const size_t index)
{
	if (index < row_count)
		return row[index];
	else
		throw std::invalid_argument("Vse perepleteno");
}
uint16_t vdb::Response::size()
{
	return row_count;
}
vdb::Response::~Response()
{
	delete[] row;
}
