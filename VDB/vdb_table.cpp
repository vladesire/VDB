#include "vdb_table.h"

vdb::Table::Table()
{
	opened = false;
}

// TODO:
vdb::Table::Table(const Table &table)
{
	//TO BE DONE
}
// TODO:
vdb::Table &vdb::Table::operator=(const vdb::Table &table)
{
	//TO BE DONE
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

bool vdb::Table::open(const std::string &name)
{
	file_name = name;

	if (name.substr(name.length()-4, 4) != ".vdb")
		file_name.append(".vdb");


	file.open(file_name, std::ios::binary | std::ios::in | std::ios::out);

	if (!file.is_open())
		return false;

	file.read((char *)&meta_size, 2);
	file.read((char *)&colcount, 1);
	file.read((char *)&rowcount, 2);

	cols = new vdb::column__[colcount];

	for (int i = 0; i < colcount; i++)
	{
		file.read((char *)&(cols[i].type), 1);
		cols[i].size = (cols[i].type == 0) ? 4 : (cols[i].type == 1 ? 8 : (cols[i].type == 2 ? 1 : cols[i].type == 3 ? 32 : 64));
		file.read((char *)&(cols[i].name), 32);
	}

	file.read((char *)&rowsize, 2);

	file.seekg(0);

	opened = true;
	return true;
}

bool vdb::Table::is_open()
{
	return opened;
}

void vdb::Table::close()
{
	file.close();
	delete[] cols;
	opened = false;
}

vdb::Response vdb::Table::select_all()
{
	vdb::Row *rows = new vdb::Row[rowcount];

	file.seekg(meta_size);
	for (size_t i = 0; i < rowcount; ++i)
	{
		rows[i].resize(colcount);
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

vdb::Response vdb::Table::select_where(std::string &condition)
{
	// All blanks must be truncated
	for (auto it = condition.begin(); it != condition.end(); ++it)
		if (*it == ' ')
			condition.erase(it--);

	Node *root = new Node;

	vdb::Response resp = select_all(); // REPLACE IT

	set_tree(condition, root, cols, colcount);

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

vdb::Response vdb::Table::select_where(const char *condition)
{
	std::string cond(condition);
	return select_where(cond);
}

void vdb::Table::clear()
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

void vdb::Table::remove_line(size_t line)
{
	if (line < 0 || line >= rowcount)
		return;

	int db_size = meta_size + (rowsize * rowcount);

	char *buffer = new char[db_size];

	file.read(buffer, db_size);

	file.close();
	file.open(file_name, std::ios::binary | std::ios::out | std::ios::in | std::ios::trunc);

	int part_1 = meta_size + (rowsize * line);
	int part_2 = rowsize * (rowcount + 1 - line);

	file.write(buffer, part_1);
	file.write(buffer + rowsize + part_1, part_2);

	delete[] buffer;

	file.seekp(3);
	--rowcount;
	file.write((char *)&rowcount, 2);
	file.seekp(0);
}

bool vdb::create_db(const std::string &desc)
{
	std::string str;
	std::fstream file;

	// pos1 ... pos2
	int pos2 = desc.find_first_of('`', 1);
	int pos1;

	// db path with name
	str = desc.substr(1, pos2 - 1);

	file.open(str, std::ios::binary | std::ios::out);

	if (!file.is_open())
		return false;

	uint16_t two_bytes;
	pos1 = pos2;
	pos2 = desc.find_first_of(' ', pos1 + 2);
	uint8_t colcount = std::stoi(desc.substr(pos1 + 2, pos2 - pos1));

	//meta size
	two_bytes = 7 + (33 * colcount);

	file.write((char *)&two_bytes, 2);
	file.write((char *)&colcount, 1);

	two_bytes = 0;
	// number of rows (= 0)
	file.write((char *)&two_bytes, 2);

	int colsize[] = {4, 8, 1, 32, 64};

	uint16_t rowsize = 0;

	std::string name;

	for (int i = 0, type; i < colcount; i++)
	{
		pos1 = pos2;
		pos2 = desc.find_first_of(' ', pos1 + 1);
		type = std::stoi(desc.substr(pos1 + 1, pos2 - pos1));
		rowsize += colsize[type];
		file.write((char *)&type, 1);
		pos1 = pos2 + 2;
		pos2 = desc.find_first_of('`', pos1);
		name = desc.substr(pos1, pos2 - pos1); // SUBSTR (pos2, LENGTH!!!!) FFFFUUUUUUUUCK
		file.write(name.c_str(), 32);
		++pos2;
	}

	// write rowsize
	file.write((char *)&rowsize, 2);

	file.close();

	return true;
}
bool vdb::create_db(const char *db_path, vdb::column *cols, uint8_t colcount)
{
	std::fstream file;
	file.open(db_path, std::ios::binary | std::ios::out);

	if (!file.is_open())
		return false;

	uint16_t two_bytes;

	//meta size
	two_bytes = 7 + (33 * colcount);

	file.write((char *)&two_bytes, 2);
	file.write((char *)&colcount, 1);

	two_bytes = 0;
	// number of rows (= 0)
	file.write((char *)&two_bytes, 2);

	int colsize[] = {4, 8, 1, 32, 64}; // int, double, char, str32, str64
	uint16_t rowsize = 0;

	for (int i = 0; i < colcount; i++)
	{
		file.write((char *)&cols[i].type, 1);
		file.write((char *)&cols[i].name, 32);
		rowsize += colsize[cols[i].type];
	}

	// write rowsize
	file.write((char *)&rowsize, 2);

	file.close();

	return true;
}