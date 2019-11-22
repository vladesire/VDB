#include <iostream>
#include "vdb_api.h"
#include <vector>

enum coltype
{
    INT = 0, DOUBLE, CHAR, STR32, STR64
};

/*
    string description structure:
    `DB_PATH_WITH_DB_NAME` COLCOUNT COLTYPE_1 `COLNAME_1` ... COLTYPE_N `COLNAME_N`
*/

#include <tuple>

template<typename... Args>
auto foo(Args... args)
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

void foo()
{
	using namespace vdb;

	Row row_1(11, 2);
	Row row_2(31, 5);
	Row row_3(187, 52);
	Row row_4(81, 56);

	Row rows[4];

	rows[0] = row_1;
	rows[1] = row_2;
	rows[2] = row_3;
	rows[3] = row_4;

	Response resp(rows, 4);

	print_response(resp);
}

int main()
{

	foo();

    std::cin.get();
    return 0;
}