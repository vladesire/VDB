#include <iostream>
#include <string>
#include "vdb_api.h"
#include "vdb_console.h"
/*
TODO LIST:

-> Forbid using fucntion when opened == false
3. Add security to select_where(), to_tree(), is_match() function to handle bad query string. (Data must not be corrupted)


-> todo: 
	vdb_row.h:35
		sizeof...() will do it.

	vdb_table.h:
		- Add move + copy-and-swap

	vdb_value
		- Arrage code nicely

	vdb_response
		- decide wether to use container

*/

#include <chrono>
#include <string>




template <class Callable, class ...Params>
auto speed_test(size_t precision, Callable func, Params ...args)
{
	using namespace std::chrono;

	auto start = steady_clock().now();
	func(args...);
	auto end = steady_clock().now();

	switch (precision)
	{
		case 0:
			return std::make_pair(std::to_string(duration_cast<milliseconds>(end - start).count()) + "ms", duration_cast<milliseconds>(end - start).count());
		case 1:
			return std::make_pair(std::to_string(duration_cast<microseconds>(end - start).count()) + "mcs", duration_cast<microseconds>(end - start).count());
		case 2:
			return std::make_pair(std::to_string(duration_cast<nanoseconds>(end - start).count()) + "ns", duration_cast<nanoseconds>(end - start).count());
		default:
			return std::make_pair(std::to_string(duration_cast<seconds>(end - start).count()) + "s", duration_cast<seconds>(end - start).count());
	}
}

void print_response(vdb::Response &&response) 
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

int main()
{
	//vdb::create_db("`Goddamn`: int `id`, double `num`");
	
	vdb::Table table;
	table.open("Goddamn");

	print_response(table.select_all());

	table.close();


	std::cin.get();
	return 0;
}
