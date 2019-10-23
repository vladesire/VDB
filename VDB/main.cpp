#include <iostream>
#include "vdb_api.h"

enum coltype
{
    INT = 0, DOUBLE, CHAR, STR32, STR64
};

/*
    string description structure:
    `DB_PATH_WITH_DB_NAME` COLCOUNT COLTYPE_1 `COLNAME_1` ... COLTYPE_N `COLNAME_N`
*/

int main()
{
    make_db("`file.vdb` 3 0 `id` 0 `weight` 3 `name`");

    meta *db = open_db("file.vdb");

    show_db(db);

    make_record(db, "1 28 `Vladesire`"); //THERE WAS A MISTAKE AFTER FIRST CLOSURE
    make_record(db, "2 34 `Vladislav`");
    make_record(db, "3 45 `Vlad`");
    make_record(db, "4 67 `Matvey`");
    make_record(db, "5 32 `Dasha`");
    make_record(db, "6 132 `Denis`");
    make_record(db, "7 62 `Gusek`");
    make_record(db, "8 36 `Nastya`");
    make_record(db, "9 30 `Liza`");
    make_record(db, "10 26 `Ilya`");

    delete_record(db, 6);

    show_db(db);

    clear_db(db);

    make_record(db, "5 32 `Dasha`");
    make_record(db, "2 34 `Vladislav`");
    make_record(db, "7 62 `Gusek`");


    show_db(db);

    delete_record(db, 2);

    show_db(db);

    close_db(db);

    std::cin.get();
    return 0;
}
