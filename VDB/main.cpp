#include <iostream>
#include "vdb_api.h"
#include <vector>

enum coltype
{
    INT = 0, DOUBLE, CHAR, STR32, STR64 // Null = 0?
};

/*
    string description structure:
    `DB_PATH_WITH_DB_NAME` COLCOUNT COLTYPE_1 `COLNAME_1` ... COLTYPE_N `COLNAME_N`
*/

#include <tuple>

template<typename... Args>
auto bar(Args... args)
{
    return std::make_tuple(args...);
}

void print_response(vdb::Response &response)
{
	for (size_t i = 0; i < response.size(); i++)
	{
		for (size_t j = 0; j < response[i].get_size(); j++)
		{
			std::cout << response[i][j].to_string() << " ";
		}
		std::cout << std::endl;
	}
}

void print_rows(vdb::Row *row, size_t size)
{
	for (size_t i = 0; i < size; ++i)
	{
		for (size_t j = 0; j < row[i].get_size(); ++j)
		{
			std::cout << row[i][j].to_string() << " ";
		}
		std::cout << std::endl;
	}
}

void foo()
{
	using namespace vdb;

	Row row_1('f', "azak");
	Row row_2("alala", 5.4);
	Row row_3(12, 'h');
	Row row_4(8841, 1488);

	Row rows[4];

	rows[0] = row_1;
	rows[1] = row_2;
	rows[2] = row_3;
	rows[3] = row_4;

	Response resp(rows, 4);

	print_response(resp);
}


// Got to be included in vdb_api later
#include "vdb_table.h"

int main()
{
	vdb::Table table;

	table.open("vladesire");

	//table.clear();

	vdb::Row row(1, 3, "Alright?");
	vdb::Row row_2(2, 3, "Alright?");
	vdb::Row row_3(3, 3, "Alright?");
	vdb::Row row_4(4, 3, "Alright?");

	table.insert_into(row);
	table.insert_into(row_2);
	table.insert_into(row_3);
	table.insert_into(row_4);

	table.remove_line(2);

	vdb::Response r = table.select().where("");

	print_response(r);

	table.close();

    std::cin.get();
    return 0;
}