#pragma once

#ifndef VDB_TABLE_H_
#define VDB_TABLE_H_

#include <cstdint>
#include <string>
#include <fstream>
#include <variant>
#include <iostream>

#include "vdb_response.h"

namespace vdb
{

struct Node
{
	std::variant<vdb::Value, uint8_t, char> value;
	struct Node *right;
	struct Node *left;
	Node()
	{
		right = left = nullptr;
	}
};
void unescape(std::string &str); // \" -> ", \' -> ', etc.
bool is_match(vdb::Row &row, const Node *tree);
void destroy_tree(struct Node *node);

typedef struct
{
	uint8_t type;
	char name[32];
	uint8_t size;
} column;

bool create_db(std::string &desc);
bool create_db(const char *desc);
bool create_db(const char *db_path, vdb::column *cols, uint8_t colcount);

class Table
{
private:
	uint16_t meta_size;
	uint8_t colcount;
	uint16_t rowcount;
	uint16_t rowsize;
	column *cols;
	std::fstream file;
	std::string file_name;

	bool opened;

	void set_tree(std::string str, Node *tree);

public:

	Table();
	Table(const Table &table);
	Table &operator=(const Table &table);
	~Table();

	bool open(const std::string &name);
	bool is_open();

	void close();

	// ONLY FOR DEBUG PURPOSES
	void print_col_names()
	{
		for (uint8_t i = 0; i < colcount; i++)
		{
			std::cout << cols[i].name << "(" << int(cols[i].type) << ")" << "\t";
		}
		std::cout << std::endl;
	}
	void meta__()
	{
		std::cout << "Meta size: \t" << meta_size << std::endl;
		std::cout << "Column count: \t" << int(colcount) << std::endl;
		std::cout << "Row count: \t" << rowcount << std::endl;
		std::cout << "Row size: \t" << rowsize << std::endl << std::endl;

		for (int i = 0, type; i < colcount; i++)
		{
			type = cols[i].type;
			std::cout << "#" << i + 1 << " Column name: " << cols[i].name << std::endl;
			std::cout << "#" << i + 1 << " Column type: " << (type == 0 ? "int" : (type == 1 ? "double" : (type == 2 ? "char" : (type == 3 ? "STR32" : "STR64")))) << std::endl;
		}

		std::cout << std::endl;
	}

	// there will be more overloaded functions
	void insert_into(Value *vals);
	void insert_into(Row &row);

	Response select_where(std::string &condition);
	Response select_where(const char *condition);

	Response select_all();
	void remove();
	void remove_line(size_t line);
	void clear();
};


}


#endif // !VDB_TABLE_H_
