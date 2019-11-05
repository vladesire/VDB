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


#include <cstdarg>

void foo(int size, ...)
{
    va_list args;
    va_start(args, size);

    while (size-- > 0)
    {
        int a = va_arg(args, int);
        std::cout << a << std::endl;
    }

    va_end(args);
}

template <typename T>
void bar(T once)
{
    std::cout << "From base: " << once << std::endl;
}

template <typename T, typename... Rest>
void bar(T first, Rest... rest)
{
    std::cout << "From recursive: " << first << std::endl;
    bar(rest...);
}


int main()
{
    //vdb::column cols[size]{{INT, "id"}, {INT, "weight"}, {CHAR, "grade"}, {STR32, "name"}};
    //vdb::make_db("testdb.vdb", cols, size);


    //bar(1, 2, 3, 4, 5, 6);


    vdb::meta *table = vdb::open_db("testdb.vdb");

    /*vdb::clear_db(table);

    vdb::make_record(table, "0 52 `A` `Mackenzie Davis_1`");
    vdb::make_record(table, "1 54 `B` `Mackenzie Davis_2`");
    vdb::make_record(table, "2 56 `C` `Mackenzie Davis_3`");*/

    //const uint8_t size = 4;
    //vdb::Value vals[size] = {3, 123, 'C', "Tarararata"};

    vdb::Row row_2(7, 220, 'Z', "Dasha! Dasha! Dasha! Dasha! Dasha! Dasha! Dasha!"); // Can't use str64


    vdb::insert_into(table, row_2);

    vdb::show_db(table);

    vdb::close_db(table);
    std::cin.get();
    return 0;
}