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
struct column
{
	uint8_t type;
	char name[32];
	uint8_t size;
};

bool create_db(std::string &desc);
bool create_db(const char *desc);
bool create_db(const char *db_path, vdb::column *cols, uint8_t colcount);

int query_cout(std::string query);

class Table
{
private:
	uint16_t meta_size;    // This is the meta. 
	uint8_t colcount;      // 
	uint16_t rowcount;     // 
	uint16_t rowsize;      // 
	column *cols;          // 
	std::fstream file;     // 
	std::string file_name; // 

	bool opened;

public:

	// Class managment
	Table();
	Table(const Table &table);
	Table &operator=(const Table &table);
	~Table();

	// File managment
	bool open(const std::string &name);
	void close();
	vdb::Response vdb_query(std::string query); // IN DEVELOPMENT

	// FOR DEBUG PURPOSES
	void print_meta()
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

	// CRUD operations:
	
	// Create
	void insert_into(Value *vals);
	void insert_into(Row &row);
	void insert_into(std::string &values);
	void insert_into(const char *values);

	// Read
	Response select_where(std::string &condition);
	Response select_where(const char *condition);
	Response select_all();
	
	// Update...

	// Delete
	void remove();
	void remove_line(size_t line);
	void clear();

	// Utils
	bool is_open() const;
	std::string get_col_name(uint8_t col_index) const;

	// Get meta information
	uint8_t get_colcount() const;
	uint16_t get_rowcount() const;

};

}
#endif // !VDB_TABLE_H_
