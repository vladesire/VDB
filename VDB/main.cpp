#include <iostream>
#include "vdb_api.h"

void print_response(vdb::Response &response)
{
	for (size_t i = 0; i < response.size(); i++)
	{
		for (size_t j = 0; j < response[i].get_size(); j++)
		{
			std::cout << response[i][j].to_string() << "\t";
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

/*
TODO LIST:

-> Test Table copy constructor and operator=()

2. In vdb_table.cpp. Investigate remove_line().
3. Add security to select_where(), to_tree(), is_match() function to handle bad query string.


*/

int main()
{
	
	/*vdb::create_db("`vladesire.vdb` 3 0 `id` 0 `age` 3 `surname`");*/
	vdb::Table table;
	table.open("vladesire");
	//table.clear();
	vdb::Row row_1(1, 12, "Vladislav");
	vdb::Row row_2(2, 12, "Vlad");
	vdb::Row row_3(3, 12, "Vladesire");
	vdb::Row row_4(4, 12, "Vika");
	vdb::Row row_5(5, 12, "Vladimir");
	vdb::Row row_6(6, 12, "Volodya");
	vdb::Row row_7(7, 12, "Vova");
	vdb::Row row_8(8, 12, "Viktor");
	vdb::Row row_9(9, 12, "Vitya");
	vdb::Row row_10(10, 12, "Valeria");
	vdb::Row row_11(11, 12, "Valera");
	vdb::Row row_12(12, 12, "Valentine");
	vdb::Row row_13(13, 12, "Veronika");
	vdb::Row row_14(14, 12, "Viktoria");
	vdb::Row row_15(15, 12, "Vadim");
	vdb::Row row_16(16, 12, "Vasiliy");
	vdb::Row row_17(17, 12, "Vasya");
	vdb::Row row_18(18, 12, "Vitaliy");
	vdb::Row row_19(19, 12, "Vissarion");
	vdb::Row row_20(20, 12, "Hank");
	
	table.insert_into(row_1);
	table.insert_into(row_2);
	table.insert_into(row_3);
	table.insert_into(row_4);
	table.insert_into(row_5);
	table.insert_into(row_6);
	table.insert_into(row_7);
	table.insert_into(row_8);
	table.insert_into(row_9);
	table.insert_into(row_10);
	table.insert_into(row_11);
	table.insert_into(row_12);
	table.insert_into(row_13);
	table.insert_into(row_14);
	table.insert_into(row_15);
	table.insert_into(row_16);
	table.insert_into(row_17);
	table.insert_into(row_18);
	table.insert_into(row_19);
	table.insert_into(row_20);
	//table.meta__();

	table.remove_line(2); // PROBLEM <---------------------------------------

	vdb::Response all = table.select_all();
	//vdb::Response r = table.select_where("`string\"32\"` !\== \"ATLAS\\\"SHRUGGED\"");
	//vdb::Response r = table.select_where(R"(`id` <= 7)");

	table.print_col_names();

	print_response(all);
	//print_response(r);

	//table.close();

	std::cin.get();
	return 0;
}
