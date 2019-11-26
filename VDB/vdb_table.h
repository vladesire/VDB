#pragma once

#ifndef VDB_TABLE_H_
#define VDB_TABLE_H_

#include <cstdint>
#include <string>
#include <fstream>

#include "vdb_response.h"

namespace vdb
{

bool create_db(const std::string &desc);
bool create_db(const char *db_path, vdb::column *cols, uint8_t colcount);

typedef struct
{
	uint8_t type;
	char name[32];
	uint8_t size;
} column__; // Name will be fixed... (And in open() function)

class Table
{
private:
	uint16_t meta_size;
	uint8_t colcount;
	uint16_t rowcount;
	uint16_t rowsize;
	column__ *cols;
	std::fstream file;
	std::string file_name;

	bool opened;

public:

	Table();
	Table(const Table &table);
	Table &operator=(const Table &table);
	~Table();

	bool open(const std::string &name);
	bool is_open();

	void close();

	// there will be more overloaded functions
	void insert_into(Value *vals);
	void insert_into(Row &row);

	Response select();
	void remove();
	void remove_line(size_t line);
	void clear();
};


}


#endif // !VDB_TABLE_H_
