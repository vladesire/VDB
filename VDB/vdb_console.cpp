#include <iostream>

#include "vdb_console.h"
#include "vdb_table.h"
#include "vdb_utils.h"

namespace 
{
	std::string next_token(std::string &source, const char *delim)
	{
		size_t pos = source.find_first_of(delim);

		if (pos == std::string::npos)
			return source;

		// To skip "pre" delimeters
		size_t skip = 0;
		if (pos == 0)
		{
			skip = source.find_first_not_of(delim);
			pos = source.find_first_of(delim, skip);
		}

		std::string temp;
		temp = source.substr(skip, pos - skip);
		source = source.substr(pos + 1);
		return temp;
	}
	void print_response(vdb::Response &response)
	{
		for (size_t i = 0; i < response.size(); ++i)
		{
			for (size_t j = 0; j < response[i].size(); ++j)
			{
				std::cout << response[i][j] << "\t";
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

	if (!query.size())
		return -38; //todo;

	if (query[0] != '`')
	{
		std::string substr = next_token(query, " ");

		if (substr == "create")
		{
			substr = next_token(query, " ");

			if (substr == "table")
			{
				if (!vdb::syntax_create_db(query))
				{
					return -1;
				}
				else
				{
					vdb::create_db(query);
					return 0;
				}
			}
			else
			{
				return 1;
			}
		}
		else if (substr == "cls")
		{
			system("cls");
			return 0;
		}
		else if (substr == "exit")
		{
			return 1812;
		}
		else
		{
			return 1;
		}
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
		return -13; //todo

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
		if (resp.size())
		{
			print_table_colnames(table);
			print_response(resp);
		}
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
			return -312; //todo
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
				std::cout << "OK!\n"; break;
			case -1:
				std::cout << "\aTable creation problems!\n"; break;
			case 1:
				std::cout << "\aUnknown command's used!\n"; break;
			case -38:
				std::cout << "\aEnter something, goddamnit!\n"; break;
			default:
				std::cout << "\aSome kind of problem\n"; break;
		}

		std::cout << "------------------------------------------------------\n\n";

		std::getline(std::cin, query_);
		std::cout << "------------------------------------------------------\n";
		r = vdb::query_cout(query_);
	}
}

