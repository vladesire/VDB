#include "vdb_table.h"
#include "vdb_utils.h"

#include <iostream> // only for exceptions
#include <sstream>
#include <cctype>
#include <regex>

struct vdb::Table::column
{
	char name[32];
	uint8_t type;
	uint8_t size;
};

namespace
{
	// small utilities
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
	inline void skip_quotes(const char *str, size_t &i, const char ch)
	{
		while (str[++i] != ch || str[i-1] == '\\'); // skip, if escaped
	}
	inline void skip_parentheses(const char *str, size_t &i)
	{
		uint8_t nest_lvl = 1;
		while (nest_lvl && ++i)
		{
			if (str[i] == ')')
				--nest_lvl;
			else if (str[i] == '(')
				++nest_lvl;
			else if (str[i] == '\"' || str[i] == '\'')
				skip_quotes(str, i, str[i]);
		}
	}
	void remove_spaces(std::string &str)
	{
		ltrim(str);
		rtrim(str);

		for (auto it = str.begin(); it != str.end(); ++it)
		{
			if (*it == '`')
			{
				while ((*(++it) != '`') && it != str.end())
					if (*it == '\\' && (it + 1) != str.end() && (it + 2) != str.end())
						++it;
			}
			else if (*it == '\"')
			{
				while ((*(++it) != '\"') && it != str.end())
					if (*it == '\\' && (it + 1) != str.end() && (it + 2) != str.end())
						++it;
			}
			else if (*it == ' ')
			{
				str.erase(it--);
			}
		}
	}
}

bool vdb::create_db(const std::string &desc)
{
	/*
		Syntax correctness is caller's duty.
		Meta layout: [meta_size][colcount][rowsize][rowcount][[coltype][colname]...]
	*/
	std::stringstream ss{trim(desc)};
	std::string filename; 

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
bool vdb::syntax_create_db(const std::string &str)
{
	std::regex pattern{R"(\s*`[^()`|&><=!]+?`\s*:(\s*,?\s*(int|char|double|str32|str64)\s*`[^()`|&><=!]+?`)+\s*)"};
	return std::regex_match(str, pattern);
}

// Select_where algorithm
bool vdb::Table::dispatch_or(const char *str, size_t l, size_t r)
{
	bool answ = false;
	size_t i = l;

	while (i < r)
	{
		if (str[i] == '|')
		{
			answ = answ || dispatch_and(str, l, i);

			if (answ)
				return answ; // if one operand of or-expression is true, the whole expression is true

			l = i + 2;
			++i;
		}
		else if (str[i] == '\"' || str[i] == '\'')
		{
			skip_quotes(str, i, str[i]);
		}
		else if (str[i] == '(')
		{
			skip_parentheses(str, i);  
		}                              
		++i;
	}

	answ = answ || dispatch_and(str, l, r);

	return answ;
}
bool vdb::Table::dispatch_and(const char *str, size_t l, size_t r)
{
	bool answ = true;
	size_t i = l;

	while (i < r)
	{
		if (str[i] == '&')
		{
			answ = answ && dispatch_comp(str, l, i);

			if (!answ)
				return answ; // if one operand of and-expression is false, the whole expression is false

			l = i + 2;
			++i;
		}
		else if (str[i] == '\"' || str[i] == '\'')
		{
			skip_quotes(str, i, str[i]);
		}
		else if (str[i] == '(')
		{
			skip_parentheses(str, i);
		}
		++i;
	}

	answ = answ && dispatch_comp(str, l, r);

	return answ;
}
bool vdb::Table::dispatch_comp(const char *str, size_t l, size_t r)
{
	if (str[l] == '(' || str[l + 1] == '(') // ( or !(
		return parentheses(str, l, r);

	size_t i = l;

	while (i < r)
	{
		if (str[i] == '>')
		{
			if (str[i + 1] == '=')
			{
				return get_val(str, l, i) >= get_val(str, i + 2, r);
			}
			else
			{
				return get_val(str, l, i) > get_val(str, i + 1, r);
			}
		}
		else if (str[i] == '<')
		{
			if (str[i + 1] == '=')
			{
				return get_val(str, l, i) <= get_val(str, i + 2, r);
			}
			else
			{
				return get_val(str, l, i) < get_val(str, i + 1, r);
			}
		}
		else if (str[i] == '=')
		{
			return get_val(str, l, i) == get_val(str, i + 2, r);
		}
		else if (str[i] == '!' && str[i + 1] == '=')
		{
			return get_val(str, l, i) != get_val(str, i + 2, r);
		}
		else if (str[i] == '\"' || str[i] == '\'')
		{
			skip_quotes(str, i, str[i]);
		}
		++i;
	}
	return false;
}
bool vdb::Table::parentheses(const char *str, size_t l, size_t r)
{
	return (str[l] == '!') ? !dispatch_or(str, l + 2, r - 1) : dispatch_or(str, l + 1, r - 1);
}
vdb::Value vdb::Table::get_val(const char *str, size_t l, size_t r)
{
	if (str[l] == '`')
	{
		std::string name {str + l + 1, r - l - 2};

		for (uint8_t i = 0; i < get_colcount(); i++)
		{
			if (get_col_name(i) == name)
			{
				return (*row_cached)[i];
			}
		}
		throw std::exception((name + " columm doesn't exists").c_str());
	}
	else if (isdigit(str[l]))
	{
		double d = strtod(str + l, nullptr);
		return Value{d};
	}
	else if (str[l] == '\"')
	{
		std::string strval {str + l + 1, r - l - 2};
		unescape(strval);
		return Value{strval.c_str()};
	}
	else if (str[l] == '\'')
	{
		return str[l+1] == '\\'? Value{str[l+2]}: Value{str[l+1]};
	}
}

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

// Create
void vdb::Table::insert_into(vdb::Row &row)
{
	if (!opened)
		return;

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
	file.seekp(6);
	++rowcount;
	file.write((char *)&rowcount, 2);
	file.seekp(0);
}
void vdb::Table::insert_into(std::string values)
{
	if (!opened)
		return;

	trim(values);
	std::stringstream ss{values};
	
	file.seekp(0, std::ios::end);

	for (size_t i = 0; i < colcount; ++i)
	{
		while (!isalnum(ss.peek()) && ss.peek() != '\"' && ss.peek() != '\'') // don't go inside strings and characters
			ss.get();

		switch (cols[i].type)
		{
			case 0:
			{
				int temp;
				ss >> temp;
				file.write((char *)&temp, sizeof(int));
			}; break;
			case 1:
			{
				double temp;
				ss >> temp;
				file.write((char *)&temp, sizeof(double));
			}; break;
			case 2:
			{
				char temp;
				ss.get(); // skip '\''
				ss >> temp;

				if(temp == '\\')
					ss >> temp;

				file.write((char *)&temp, sizeof(char));
			}; break;
			case 3:
			case 4:
			{
				std::string temp;
				get_name(ss, temp, '\"');

				file.write(temp.c_str(), cols[i].size * sizeof(char));
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
	if (!opened)
		throw std::exception("Table is not opened!");

	vdb::Response resp;
	vdb::Row row;
	row.reserve(colcount);

	file.seekg(meta_size);
	for (size_t i = 0; i < rowcount; ++i)
	{
		for (int j = 0; j < colcount; ++j)
		{
			switch (cols[j].type)
			{
				case 0:
				{
					int buff;
					file.read((char *)&buff, sizeof(int));
					row.push_back(buff);
				}; break;
				case 1:
				{
					double buff;
					file.read((char *)&buff, sizeof(double));
					row.push_back(buff);
				}; break;
				case 2:
				{
					char buff;
					file.read(&buff, sizeof(char));
					row.push_back(buff);
				}; break;
				case 3:
				{
					char buff[32];
					file.read(buff, 32 * sizeof(char));
					row.push_back(buff);
				}; break;
				case 4:
				{
					char buff[64];
					file.read(buff, 64 * sizeof(char));
					row.push_back(buff);
				}; break;
			}
		}
		resp.push_back(row);
		row.clear();
	}

	file.seekg(0);
	return resp;
}
vdb::Response vdb::Table::select_where(std::string condition)
{
	if (!opened)
		throw std::exception("Table is not opened!");

	remove_spaces(condition);

	vdb::Response all = select_all();
	vdb::Response resp;

	auto cond = condition.c_str();
	auto len = condition.size();

	try
	{
		for (uint16_t i = 0; i < all.size(); ++i)
		{
			row_cached = &all[i];

			if (dispatch_or(cond, 0, len))
			{
				resp.push_back(all[i]);
			}
		}
	}
	catch (const std::exception &ex)
	{
		std::cout << "\n" << ex.what() << "\n";
		return Response();
	}

	return resp;
}

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

		buff[6] = 0; // to set rowcount zero
		buff[7] = 0;

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
uint16_t vdb::Table::get_colcount() const
{
	return opened? colcount: 0;
}
uint16_t vdb::Table::get_rowcount() const
{
	return opened ? rowcount : 0;
}
