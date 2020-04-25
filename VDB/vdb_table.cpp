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
	void get_name(std::stringstream &ss, std::string &to, char border)
	{ 
		// skip everything till border
		while (ss.peek() != border)
			ss.get();
		ss.get();

		while (ss.peek() != border)
		{
			if (ss.peek() == '\\') // skip leading '\'
				ss.get();
			to += ss.get();
		}
		ss.get(); // skip last border
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

	get_name(ss, filename, '`'); // `name with possible \` char`
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
		get_name(ss, colinfo, '`'); // get colname

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
void vdb::Table::insert_into(vdb::Row &row)
{
	file.seekp(0, std::ios::end);

	for (int i = 0; i < colcount; i++)
	{
		if (row[i].type() != cols[i].type)
		{
			// Null value must be set if it's possible (not_null = false)

			char a = 0;

			for (int k = 0; k < cols[i].size; ++k)
				file.write(&a, 1);

			continue;
		}
		switch (cols[i].type)
		{
			case 0:
				file.write(row[i].cptr(), 4); break;
			case 1:
				file.write(row[i].cptr(), 8); break;
			case 2:
				file.write(row[i].cptr(), 1); break;
			case 3:
				file.write(row[i].cptr(), 32); break;
			case 4:
				file.write(row[i].cptr(), 64); break;
		}
	}
	file.seekp(3);
	++rowcount;
	file.write((char *)&rowcount, 2);
	file.seekp(0);
}

void vdb::Table::insert_into(std::string values)
{
	trim(values);
	std::stringstream ss{values};
	
	file.seekp(0, std::ios::end);

	for (size_t i = 0; i < colcount; ++i)
	{
		while (!isalnum(ss.peek()))
			ss.get();

		switch (cols[i].type)
		{
			case 0:
			{
				int temp;
				ss >> temp;
				file.write((char *)&temp, 4);
			}; break;
			case 1:
			{
				double temp;
				ss >> temp;
				file.write((char *)&temp, 8);
			}; break;
			case 2:
			{
				char temp;
				ss >> temp;

				if(temp == '\\')
					ss >> temp;

				file.write((char *)&temp, 1);
			}; break;
			case 3:
			case 4:
			{
				std::string temp;
				ss.unget();
				get_name(ss, temp, '\"');

				file.write(temp.c_str(), cols[i].size);
			}; break;
		}
	}

	file.seekp(6);
	++rowcount;
	file.write((char *)&rowcount, 2);
	file.seekp(0);
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