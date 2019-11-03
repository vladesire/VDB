#ifndef UTILS_H
#define UTILS_H

#include "meta.h"
#include "crud.h"


namespace vdb
{
    void show_record(meta *db, vrecord &vec);
    void show_meta(meta *db);
    void show_db(meta *db);
}
#endif // !UTILS_H
