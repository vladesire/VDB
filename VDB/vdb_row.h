#ifndef ROW_H
#define ROW_H

#include <cstdint>
#include "vdb_value.h"
#include "meta.h"

namespace vdb
{/*
    class Row
    {
    private:
        vdb::ValueBase *row = nullptr;
        uint8_t max, index = 0;
    public:
        Row(meta *db)
        {
            max = db->colcount;
            row = new ValueBase[max];
        }
        ~Row()
        {
            delete[] row;
        }
        template <typename T>
        void push_back(vdb::Value<T> val)
        {
            if (index >= max)
                return;
            row[index++] = val;
        }

        ValueBase &operator[] (uint8_t i)
        {
            if (i >= max)
                throw std::invalid_argument("Index is out of bounds!");
            return row[i];
        }

    };
    */
}
#endif