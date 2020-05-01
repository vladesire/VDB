#include <iostream>
#include <string>
#include "vdb_api.h"
#include "vdb_console.h"
/*
TODO LIST:

-> Forbid using fucntion when opened == false
3. Add security to select_where(), to_tree(), is_match() function to handle bad query string. (Data must not be corrupted)


-> todo: 
	vdb_value
		- Arrage code nicely

	vdb_utils
		- add namespave vdb_impl or something

*/

#include <chrono>
auto get_diff_str(std::chrono::steady_clock::time_point start, std::chrono::steady_clock::time_point end, size_t precision = 1)
{
	using namespace std::chrono;

	switch (precision)
	{
		case 1:
			return std::to_string(duration_cast<milliseconds>(end - start).count()) + "ms";
		case 2:
			return std::to_string(duration_cast<microseconds>(end - start).count()) + "mcs";
		case 3:
			return std::to_string(duration_cast<nanoseconds>(end - start).count()) + "ns";
		default:
			return std::to_string(duration_cast<seconds>(end - start).count()) + "s";
	}
}
auto get_diff_num(std::chrono::steady_clock::time_point start, std::chrono::steady_clock::time_point end, size_t precision = 1)
{
	using namespace std::chrono;

	switch (precision)
	{
		case 1:
			return duration_cast<milliseconds>(end - start).count();
		case 2:
			return duration_cast<microseconds>(end - start).count();
		case 3:
			return duration_cast<nanoseconds>(end - start).count();
		default:
			return duration_cast<seconds>(end - start).count();
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
	// todo: test select_where() again. I used util function in it.
	
	std::cin.get();
	return 0;
}
