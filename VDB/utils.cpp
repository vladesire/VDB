#include "utils.h"

void vdb::show_record(vdb::meta *db, vdb::vrecord &vec)
{
    for (int i = 0; i < db->colcount; i++)
        std::cout << vec[i].to_string() << "\t";
    std::cout << std::endl;

}

void vdb::show_meta(vdb::meta *db)
{
    std::cout << "Meta size: \t" << db->meta_size << std::endl;
    std::cout << "Column count: \t" << int(db->colcount) << std::endl;
    std::cout << "Row count: \t" << db->rowcount << std::endl;
    std::cout << "Row size: \t" << db->rowsize << std::endl << std::endl;

    for (int i = 0, type; i < db->colcount; i++)
    {
        type = db->cols[i].type;
        std::cout << "#" << i + 1 << " Column name: " << db->cols[i].name << std::endl;
        std::cout << "#" << i + 1 << " Column type: " << (type == 0 ? "int" : (type == 1 ? "double" : (type == 2 ? "char" : (type == 3 ? "STR32" : "STR64")))) << std::endl;
    }

    std::cout << std::endl;
}

void vdb::show_db(vdb::meta *db)
{
    for (int i = 0; i < db->colcount; i++)
    {
        std::cout << db->cols[i].name << "\t";
    }
    std::cout << std::endl;
    for (int i = 0; i < db->rowcount; i++)
    {
        vrecord rec;
        get_record(db, rec, i);
        vdb::show_record(db, rec);
    }
}