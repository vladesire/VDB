#ifndef META_H
#define META_H

#include <vector>
#include <iostream>
#include <cstdint>
#include "vdb_value.h"
namespace vdb
{
    typedef struct
    {
        uint8_t type;
        char name[32];
    } column;

    typedef struct
    {
        uint16_t meta_size;
        uint8_t colcount;
        uint16_t rowcount;
        uint16_t rowsize;
        column *cols;
        std::fstream *file;
        std::string file_name;
    } meta;

    typedef std::vector<vdb::Value> vrecord;
}
#endif // !META_H
