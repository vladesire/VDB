#include "vdb_table.h"
#include "vdb_query_proc.h"
#include "vdb_utils.h"

#include <sstream>
#include <cctype>

struct vdb::Table::column
{
	char name[32];
	uint8_t type;
	uint8_t size;
};

namespace
{
	void get_name(std::stringstream &ss, std::string &to)
	{ 
		// skip everything till `
		while (ss.peek() != '`')
			ss.get();
		ss.get();

		while (ss.peek() != '`')
		{
			if (ss.peek() == '\\') // skip escaped char
				ss.get();
			else
				to += ss.get();
		}
		ss.get(); // skip last `
		ss.peek(); // to turn on eof bit (if it's the end)
	}

	size_t col_type(std::string &name)
	{
		if (name == "int")
			return 0;
		else if (name == "char")
			return 2;
		else if (name == "str32")
			return 3;
		else if (name == "str64")
			return 4;
		else
			return 1; // if double or anything else
	}
}

bool vdb::create_db(std::string desc)
{
	/*
		Syntax correctness is caller's duty.
		Meta layout: [meta_size][colcount][rowsize][rowcount][[coltype][colname]...]
	*/
	trim(desc);
	std::stringstream ss{desc};
	std::string filename; // use it as filename

	get_name(ss, filename); // `name with possible \` char`
	unescape(filename);
	filename.append(".vdb");

	std::fstream file(filename, std::ios::binary | std::ios::out);

	if (!file.is_open())
		return false;

	uint16_t meta_size, colcount = 0, rowsize = 0;

	file.write((char *)&rowsize, 8); // skip this for now (will be rewritten)

	int colsize[] = {4, 8, 1, 32, 64};

	std::string colinfo; 
	uint8_t coltype;

	while (!ss.eof() && ss) // while no troubles occured
	{
		while (!isalpha(ss.peek()))
			ss.get();

		ss >> colinfo; // get literal coltype
		coltype = col_type(colinfo);  

		colinfo.clear();
		get_name(ss, colinfo); // get colname
		unescape(colinfo);

		file.write((char *)&coltype, 1);
		file.write(colinfo.c_str(), 32);

		++colcount;
		rowsize += colsize[coltype];
	}

	meta_size = 8 + (33 * colcount);

	file.seekp(0);

	file.write((char *)&meta_size, 2);
	file.write((char *)&colcount, 2);
	file.write((char *)&rowsize, 2);
	
	rowsize = 0; //use it as rowcount (= 0)
	file.write((char *)&rowsize, 2);

	file.close();
	return true;
}

//todo DEBUG --- start ---
#include <iostream>
void vdb::Table::print_meta()
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
//todo DEBUG --- end ---



// Class managment
vdb::Table::Table() : opened{false} { }

vdb::Table::Table(const Table &table)
{
	if (!table.opened)
	{
		opened = false;
	}
	else
	{
		meta_size = table.meta_size;
		colcount = table.colcount;
		rowcount = table.rowcount;
		rowsize = table.rowsize;

		cols = new column[colcount];

		for (size_t i = 0; i < colcount; ++i)
		{
			strcpy(cols[i].name, table.cols[i].name);
			cols[i].type = table.cols[i].type;
			cols[i].size = table.cols[i].size;
		}

		file_name = table.file_name;

		if (file_name.substr(file_name.length() - 4, 4) != ".vdb")
			file_name.append(".vdb");

		file.open(file_name, std::ios::binary | std::ios::in | std::ios::out);

		opened = true;
	}
}
vdb::Table &vdb::Table::operator=(const vdb::Table &table)
{
	if (this == &table)
		return *this;
	if (!table.opened)
	{	
		opened = false;
	}
	else
	{
		meta_size = table.meta_size;
		colcount = table.colcount;
		rowcount = table.rowcount;
		rowsize = table.rowsize;

		cols = new column[colcount];

		for (size_t i = 0; i < colcount; ++i)
		{
			strcpy(cols[i].name, table.cols[i].name);
			cols[i].type = table.cols[i].type;
			cols[i].size = table.cols[i].size;
		}

		file_name = table.file_name;

		if (file_name.substr(file_name.length() - 4, 4) != ".vdb")
			file_name.append(".vdb");

		file.open(file_name, std::ios::binary | std::ios::in | std::ios::out);

		opened = true;
	}
	return *this;
}
vdb::Table::~Table()
{
	if (opened)
	{ 
		file.close();
		delete[] cols;
	}
}


// File managment
bool vdb::Table::open(const std::string &name)
{
	file_name = name;

	if (name != ".settings" && name.substr(name.length() - 4, 4) != ".vdb")
		file_name.append(".vdb");

	file.open(file_name, std::ios::binary | std::ios::in | std::ios::out);

	if (!file.is_open())
		return false;

	file.read((char *)&meta_size, 2);
	file.read((char *)&colcount, 2);
	file.read((char *)&rowsize, 2);
	file.read((char *)&rowcount, 2);
	

	cols = new column[colcount];

	for (int i = 0; i < colcount; i++)
	{
		file.read((char *)&(cols[i].type), 1);
		cols[i].size = (cols[i].type == 0) ? 4 : (cols[i].type == 1 ? 8 : (cols[i].type == 2 ? 1 : cols[i].type == 3 ? 32 : 64));
		file.read((char *)&(cols[i].name), 32);
	}

	
	file.seekg(0);

	opened = true;
	return true;
}
void vdb::Table::close()
{
	if (opened)
	{
		file.close();
		delete[] cols;
		opened = false;
	}
}
vdb::Response vdb::Table::vdb_query(std::string query)
{
	// IN DEVELOPMENT
	return vdb::Response();
}


// CRUD operations:

// Create
void vdb::Table::insert_into(vdb::Value *vals)
{
	// TODO: check if vals is valid (colcount) + think about auto-value in row (like null by default or autoincrement)
	file.seekp(0, std::ios::end);

	for (int i = 0; i < colcount; i++)
	{
		if (vals[i].get_type() != cols[i].type)
		{
			// Null value must be set if its possible (not_null = false)

			char a = 0;

			for (int i = 0; i < cols[i].size; ++i)
				file.write(&a, 1);
		}
		switch (cols[i].type)
		{
			// (char *)vals[i] will return pointer to vdb::Value inner buffer that contain the value
			case 0:
				file.write((char *)vals[i], 4); break;
			case 1:
				file.write((char *)vals[i], 8); break;
			case 2:
				file.write((char *)vals[i], 1); break;
			case 3:
				file.write((char *)vals[i], 32); break;
			case 4:
				file.write((char *)vals[i], 64); break;
		}
	}
	file.seekp(3);
	++rowcount;
	file.write((char *)&rowcount, 2);
	file.seekp(0);
}
void vdb::Table::insert_into(vdb::Row &row)
{
	// TODO: check if vals is valid (colcount) + think about auto-value in row (like null by default or autoincrement)
	file.seekp(0, std::ios::end);

	for (int i = 0; i < colcount; i++)
	{
		if (row[i].get_type() != cols[i].type)
		{
			// Null value must be set if its possible (not_null = false)

			char a = 0;

			for (int k = 0; k < cols[i].size; ++k)
				file.write(&a, 1);

			continue;
		}
		switch (cols[i].type)
		{
			// (char *)vals[i] will return pointer to vdb::Value inner buffer that contain the value
			case 0:
				file.write((char *)row[i], 4); break;
			case 1:
				file.write((char *)row[i], 8); break;
			case 2:
				file.write((char *)row[i], 1); break;
			case 3:
				file.write((char *)row[i], 32); break;
			case 4:
				file.write((char *)row[i], 64); break;
		}
	}
	file.seekp(3);
	++rowcount;
	file.write((char *)&rowcount, 2);
	file.seekp(0);
}
void vdb::Table::insert_into(std::string &values)
{

	file.seekp(0, std::ios::end);
	// Values example: "12, 43.34, \"String\", \'C\'"
	ltrim(values); // without it mechanism crashes because iterator can't be decremented under the condition.begin()
	rtrim(values);
	for (auto it = values.begin(); it != values.end(); ++it)
	{
		if (*it == '`')
		{
			while (*(++it) != '`')
				if (*it == '\\' && (it + 1) != values.end() && (it + 2) != values.end())
					++it;
		}
		else if (*it == '\"')
		{
			while (*(++it) != '\"')
				if (*it == '\\' && (it + 1) != values.end() && (it + 2) != values.end())
					++it;
		}
		else if (*it == ' ')
		{
			values.erase(it--);
		}
	}

	for (size_t i = 0; i < colcount; ++i)
	{
		size_t k = 0;
		for (; values[k] != ',' && k < values.length(); ++k)
		{
			if (values[k] == '\"')
			{
				while (values[++k] != '\"')
				{
					if (values[k] == '\\' && (k + 1) < values.length() && (k + 2) != values.length())
						++k;
				}
			}
			if (values[k] == '\'')
			{
				if(values[k + 1] == '\\')
					k += 3;
				else
					k += 2;
			}
			
		}
		std::string str_val = values.substr(0, k);
		if (k < values.length())
			values = values.substr(k + 1);

		switch (cols[i].type)
		{
			case 0:
			{
				int val = std::stoi(str_val);
				file.write((char *)&val, 4);
			}; break;
			case 1:
			{
				double val = std::stod(str_val);
				file.write((char *)&val, 8);
			}; break;
			case 2:
			{
				if (str_val[1] == '\\')
					file.write((char *)&str_val[2], 1);
				else 
					file.write((char *)&str_val[1], 1);
			}; break;
			case 3:
			case 4:
			{
				unescape(str_val);
				file.write(str_val.substr(1, str_val.length() - 2).c_str(), cols[i].size);
			}; break;

			default:
				break;
		}

	}
	file.seekp(3);
	++rowcount;
	file.write((char *)&rowcount, 2);
	file.seekp(0);
}
void vdb::Table::insert_into(const char *values)
{
	std::string vals(values);
	insert_into(vals);
}

// Read
vdb::Response vdb::Table::select_all()
{
	vdb::Row *rows = new vdb::Row[rowcount];

	file.seekg(meta_size);
	for (size_t i = 0; i < rowcount; ++i)
	{
		rows[i].reserve(colcount);
		for (int j = 0; j < colcount; ++j)
		{
			switch (cols[j].type)
			{
				case 0:
				{
					int buff;
					file.read((char *)&buff, 4);
					rows[i].push_back(buff);
				}; break;
				case 1:
				{
					double buff;
					file.read((char *)&buff, 8);
					rows[i].push_back(buff);
				}; break;
				case 2:
				{
					char buff;
					file.read(&buff, 1);
					rows[i].push_back(buff);
				}; break;
				case 3:
				{
					char buff[32];
					file.read(buff, 32);
					rows[i].push_back(buff);
				}; break;
				case 4:
				{
					char buff[64];
					file.read(buff, 64);
					rows[i].push_back(buff);
				}; break;
			}
		}
	}


	vdb::Response resp(rows, rowcount);

	file.seekg(0);
	delete[] rows;

	return resp;
}
// TODO INSIDE!!!
vdb::Response vdb::Table::select_where(std::string condition)
{
	using namespace vdb_impl;

	// All excess blanks must be truncated
	ltrim(condition); // without it mechanism crashes because iterator can't be decremented under the condition.begin()
	for (auto it = condition.begin(); it != condition.end(); ++it) // TODO: The leading blank will fuck my algorithm
	{
		if (*it == '`')
		{
			while (*(++it) != '`')
				if (*it == '\\' && (it + 1) != condition.end() && (it + 2) != condition.end())
					++it;
		}
		else if (*it == '\"')
		{
			while (*(++it) != '\"')
				if (*it == '\\' && (it + 1) != condition.end() && (it + 2) != condition.end())
					++it;
		}
		else if (*it == ' ')
		{
			condition.erase(it--);
		}
	}
	Node *root = new Node;

	vdb::Response resp = select_all();

	set_tree(condition, root, *this);

	uint16_t match_count = 0;
	vdb::Row *match_indexes = new vdb::Row[resp.size()]; // I SHOULD STORE ONLY POINTERS TO THE ROWS, NOT VALUES ITSELF (OR NOT???)

	for (uint16_t i = 0; i < resp.size(); i++)
		if (is_match(resp[i], root))
			match_indexes[match_count++] = resp[i]; // sizeof(vdb::Row) > sizeof(vdb::Row *), but should I define operator= for pointers copy?

	vdb::Response new_response(match_indexes, match_count);

	resp = new_response;

	delete[] match_indexes;
	destroy_tree(root);

	return resp;

	return resp;
}


// Update...


// Delete
void vdb::Table::clear()
{
	if (opened)
	{
		char *buff = new char[meta_size];

		file.read(buff, meta_size);

		file.close();
		file.open(file_name, std::ios::binary | std::ios::out | std::ios::in | std::ios::trunc);

		rowcount = 0;

		buff[3] = 0; // to set rowcount zero
		buff[4] = 0;

		file.write(buff, meta_size);
		file.seekp(0);
		delete[] buff;
	}
}
void vdb::Table::remove_line(size_t line)
{
	if (line < 0 || line >= rowcount || !opened) // line cannot be negative (size_t), maybe in the future this can be useful
		return;

	size_t db_size = meta_size + (rowsize * rowcount);

	char *buffer = new char[db_size];

	file.read(buffer, db_size);

	file.close();
	file.open(file_name, std::ios::binary | std::ios::out | std::ios::in | std::ios::trunc);

	size_t part_1 = meta_size + (rowsize * line);
	size_t part_2 = db_size - part_1 - rowsize;

	file.write(buffer, part_1);
	file.write(buffer + part_1 + rowsize, part_2);

	delete[] buffer;

	file.seekp(3);
	--rowcount;
	file.write((char *)&rowcount, 2);
	file.seekp(0);
}

// Utils
std::string vdb::Table::get_col_name(uint8_t col_index) const
{
	if (opened && (col_index < colcount))
		return std::string(cols[col_index].name);
	else
		throw std::invalid_argument("Argument is more than colcount or table is not opened!");
}
bool vdb::Table::is_open() const
{
	return opened;
}

// Get meta information
uint16_t vdb::Table::get_colcount() const
{
	return opened? colcount: 0;
}
uint16_t vdb::Table::get_rowcount() const
{
	return opened ? rowcount : 0;
}