#include <iostream>
#include <string>
#include "vdb_api.h"

/*
TODO LIST:

-> Forbid using fucntion when opened == false
3. Add security to select_where(), to_tree(), is_match() function to handle bad query string. (Data must not be corrupted)

*/

//#include <filesystem> for future purposes

void start_vdbms()
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

int main()
{
	start_vdbms();

	return 0;
}
