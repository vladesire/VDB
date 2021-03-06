#ifndef VDB_TABLE_H_
#define VDB_TABLE_H_

#include <cstdint>
#include <string>
#include <fstream>

#include "vdb_response.h"

namespace vdb
{

bool create_db(const std::string &description); 
bool syntax_create_db(const std::string &str);

class Table
{
public:
	Table();
	Table(const Table &table) = delete;
	Table(const Table &&table) = delete;
	Table &operator=(const Table table) = delete;
	~Table();

	// File managment
	bool open(const std::string &name);
	void close();

	// Create
	void insert_into(Row &row);
	void insert_into(std::string values);

	// Read
	Response select_where(std::string condition);
	Response select_all();

	// Delete
	void remove_line(size_t line);
	void clear();

	// Utils
	bool is_open() const;
	uint16_t get_colcount() const;
	uint16_t get_rowcount() const;
	std::string get_col_name(uint8_t col_index) const;

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

	// Select where algorithm
	mutable Row *row_cached;

	bool dispatch_or(const char *query, size_t l, size_t r);
	bool dispatch_and(const char *query, size_t l, size_t r);
	bool dispatch_comp(const char *query, size_t l, size_t r);
	Value get_val(const char *query, size_t l, size_t r);
	inline bool parentheses(const char *query, size_t l, size_t r);
};

}
#endif // !VDB_TABLE_H_
