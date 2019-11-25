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

vdb::Response vdb::Table::select()
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

	// IT DOESN'T WORK!!!
	for (int i = 0; i < colcount; i++)
	{
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

	// IT DOESN'T WORK!!!
	for (int i = 0; i < colcount; i++)
	{
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