#include <iostream>

#include "vdb_console.h"
#include "vdb_table.h"
#include "vdb_utils.h"

namespace 
{ 
void print_response(vdb::Response &response)
{
	for (size_t i = 0; i < response.size(); i++)
	{
		for (size_t j = 0; j < response[i].size(); j++)
		{
			std::cout << response[i][j] << "\t";
		}
		std::cout << std::endl;
	}
}
// Should I delete print_rows?
void print_rows(vdb::Row *row, size_t size)
{
	for (size_t i = 0; i < size; ++i)
	{
		for (size_t j = 0; j < row[i].size(); ++j)
		{
			std::cout << row[i][j] << " ";
		}
		std::cout << std::endl;
	}
}
void print_table_colnames(const vdb::Table &table)
{
	for (size_t i = 0; i < table.get_colcount(); ++i)
	{
		std::cout << table.get_col_name(i) << "\t";
	}
	std::cout << std::endl;
}
}
int vdb::query_cout(std::string query)
{
	ltrim(query);

	if (query[0] != '`')
	{
		std::string substr = next_token(query, " ");

		if (substr == "create")
		{
			substr = next_token(query, " ");

			if (substr == "table")
			{
				vdb::create_db(query);
				return 0;
			}
			else
				return 2;
		}
		else if (substr == "cls")
		{
			system("cls");
			return 0;
		}
		else if (substr == "exit")
			return 1812;
		else
			return 2;
	}

	size_t i = 0;
	while (query[++i] != '`')
		if (query[i] == '\\' && (i + 1) < query.length() && (i + 2) != query.length())
			++i;

	std::string table_name = query.substr(1, i - 1);

	query = query.substr(i + 2);

	unescape(table_name);

	vdb::Table table;
	table.open(table_name);

	if (!table.is_open())
		return -1;

	std::string substr = next_token(query, " ");

	if (substr == "select_all")
	{
		vdb::Response resp = table.select_all();
		print_table_colnames(table);
		print_response(resp);
	}
	else if (substr == "insert_into")
	{
		table.insert_into(query);
	}
	else if (substr == "select_where")
	{
		vdb::Response resp = table.select_where(query);
		print_table_colnames(table);
		print_response(resp);
	}
	else if (substr == "clear")
	{
		table.clear();
	}
	else if (substr == "remove_line")
	{
		try
		{
			size_t line = std::stoi(query);
			table.remove_line(line);
		}
		catch (const std::exception &)
		{
			std::cout << "Fuck you, asshole!";
		}
	}
	else if (substr == "delete")
	{
		table.close();
		std::string query("del ");
		query.append(table_name);
		if (table_name.substr(table_name.length() - 4, 4) != ".vdb")
			query.append(".vdb");
		std::system(query.c_str());
		return 0;
	}

	table.close();
	return 0;
}

void vdb::vdbms_cout()
{
	/*vdb::Table settings;
	if (std::filesystem::exists(".settings"));
	{
		settings.open(".settings");
		//LOAD SETTINGS
	}*/

	//std::system("cd databases"); ????

	std::string query_;

	std::getline(std::cin, query_);
	std::cout << "------------------------------------------------------\n";

	int r = vdb::query_cout(query_);

	while (r != 1812) // exit code (my birthday)
	{
		std::cout << "------------------------------------------------------\n";
		switch (r)
		{
			case 0:
				std::cout << "OK!" << std::endl; break;
			case -2:
				std::cout << "\aWRONG_SYNTAX!" << std::endl; break;
			default:
				std::cout << "\aSome kind of problem" << std::endl; break;
		}

		std::cout << "------------------------------------------------------\n\n";

		std::getline(std::cin, query_);
		std::cout << "------------------------------------------------------\n";
		r = vdb::query_cout(query_);
	}
}

