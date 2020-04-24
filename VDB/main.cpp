#include <iostream>
#include <string>
#include "vdb_api.h"
/*
TODO LIST:

-> Forbid using fucntion when opened == false
3. Add security to select_where(), to_tree(), is_match() function to handle bad query string. (Data must not be corrupted)


-> todo: 
	vdb_row.h:35
		sizeof...() will do it.

	vdb_table.h:22
		make one create_db(std::string), delete version with columns

	vdb_value
		- Arrage code nicely

	vdb_response
		- decide wether to use container

*/


int main()
{
	//vdb::vdbms_cout();

	vdb::create_db("`Goddamn`: int `crap`, double `hellyeah`, char `that's it`");

	vdb::Table table;

	table.open("Goddamn");

	table.print_meta();

	table.close();

	std::cin.get();
	return 0;
}
