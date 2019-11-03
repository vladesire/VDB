#ifndef VDB_CRUD_H
#define VDB_CRUD_H

#include <string>
#include <fstream>
#include "meta.h"

namespace vdb
{
    bool make_db(const std::string &desc);
    meta *open_db(const char *db_path);
    void close_db(meta *db);

    void make_record(meta *db, const std::string &record);
    void get_record(meta *db, vrecord &vec, int line);
    void delete_record(meta *db, int line);
    void clear_db(meta *db);
}
#endif // !VDB_CRUD_H
