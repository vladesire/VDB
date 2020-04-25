#ifndef VDB_TABLE_H_
#define VDB_TABLE_H_

#include <cstdint>
#include <string>
#include <fstream>

#include "vdb_response.h"

namespace vdb
{

bool create_db(std::string description); 

class Table
{
private:
	struct column;

	bool opened;
	uint16_t meta_size;    // This is the meta. 
	uint16_t colcount;     // 
	uint16_t rowcount;     // 
	uint16_t rowsize;      // 
	column *cols;          // 
	std::string file_name; // 
	std::fstream file;     // 

public:

	// Class managment
	Table();
	Table(const Table &table) = delete;
	Table(const Table &&table) = delete;
	Table &operator=(const Table table) = delete;
	
	~Table();

	// File managment
	bool open(const std::string &name);
	void close();
	vdb::Response vdb_query(std::string query); // IN DEVELOPMENT

	// Utils
	bool is_open() const;
	uint16_t get_colcount() const;
	uint16_t get_rowcount() const;
	std::string get_col_name(uint8_t col_index) const;

	// FOR DEBUG PURPOSES
	void print_meta();

	// CRUD operations:
	
	// Create
	void insert_into(Row &row);
	void insert_into(std::string values);

	// Read
	Response select_where(std::string condition);
	Response select_all();
	
	// Update...

	// Delete
	void remove();
	void remove_line(size_t line);
	void clear();
};

}
#endif // !VDB_TABLE_H_
