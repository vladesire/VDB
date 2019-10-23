#include "crud.h"

bool make_db(const std::string &desc)
{
    std::string str;
    std::fstream file;

    // pos1 ... pos2
    int pos2 = desc.find_first_of('`', 1);
    int pos1;

    // db path with name
    str = desc.substr(1, pos2 - 1);

    file.open(str, std::ios::binary | std::ios::out);

    if (!file.is_open())
        return false;

    uint16_t two_bytes;
    pos1 = pos2;
    pos2 = desc.find_first_of(' ', pos1 + 2);
    uint8_t colcount = std::stoi(desc.substr(pos1 + 2, pos2 - pos1));

    //meta size
    two_bytes = 7 + (33 * colcount);

    file.write((char *)&two_bytes, 2);
    file.write((char *)&colcount, 1);

    two_bytes = 0;
    // number of rows (= 0)
    file.write((char *)&two_bytes, 2);

    int colsize[] = {4, 8, 1, 32, 64};

    uint16_t rowsize = 0;

    std::string name;

    for (int i = 0, type; i < colcount; i++)
    {
        pos1 = pos2;
        pos2 = desc.find_first_of(' ', pos1 + 1);
        type = std::stoi(desc.substr(pos1 + 1, pos2 - pos1));
        rowsize += colsize[type];
        file.write((char *)&type, 1);
        pos1 = pos2 + 2;
        pos2 = desc.find_first_of('`', pos1);
        name = desc.substr(pos1, pos2 - pos1); // SUBSTR (pos2, LENGTH!!!!) FFFFUUUUUUUUCK
        file.write(name.c_str(), 32);
        ++pos2;
    }

    // write rowsize
    file.write((char *)&rowsize, 2);

    file.close();

    return true;
}
meta *open_db(const char *db_path)
{
    meta *m = new meta;

    m->file_name = std::string(db_path);

    m->file = new std::fstream;

    m->file->open(db_path, std::ios::binary | std::ios::in | std::ios::out);

    if (!m->file->is_open())
        return nullptr;

    m->file->read((char *)&m->meta_size, 2);
    m->file->read((char *)&m->colcount, 1);
    m->file->read((char *)&m->rowcount, 2);

    m->cols = new column[m->colcount];

    for (int i = 0; i < (m->colcount); i++)
    {
        m->file->read((char *)&m->cols[i].type, 1);
        m->file->read((char *)&m->cols[i].name, 32);
    }

    m->file->read((char *)&m->rowsize, 2);

    m->file->seekg(0);

    return m;
}
void close_db(meta *db)
{
    db->file->close();
    delete db->file;
    delete[] db->cols;
    delete db;
}

void make_record(meta *db, const std::string &record)
{
    db->file->seekp(0, std::ios::end);

    int pos1 = 0, pos2 = 0;

    for (int i = 0; i < db->colcount; i++)
    {
        switch (db->cols[i].type)
        {
            case 0:
            {
                pos2 = record.find_first_of(' ', pos1);
                uint32_t four_bytes = stoi(record.substr(pos1, pos2 - pos1));
                db->file->write((char *)&four_bytes, 4);
                pos1 = pos2 + 1;
            }; break;
            case 1:
            {
                pos2 = record.find_first_of(' ', pos1);
                double eight_bytes = stod(record.substr(pos1, pos2 - pos1));
                db->file->write((char *)&eight_bytes, 8);
                pos1 = pos2 + 1;
            }; break;
            case 2:
            {
                pos2 = record.find_first_of('`', ++pos1);
                uint8_t byte = record.substr(pos1, pos2 - pos1)[0];
                db->file->write((char *)&byte, 1);
                pos1 = pos2 + 2;
            }; break;
            case 3:
            {
                pos2 = record.find_first_of('`', ++pos1);
                db->file->write(record.substr(pos1, pos2 - pos1).c_str(), 32);
                pos1 = pos2 + 2;
            }; break;
            case 4:
            {
                pos2 = record.find_first_of('`', ++pos1);
                db->file->write(record.substr(pos1, pos2 - pos1).c_str(), 64);
                pos1 = pos2 + 2;
            }; break;
        }
    }
    db->file->seekp(3);
    ++db->rowcount;
    db->file->write((char *)&db->rowcount, 2);
    db->file->seekp(0);
}
void get_record(meta *db, vrecord &vec, int line)
{
    db->file->seekg(db->meta_size + (line * db->rowsize));

    for (int i = 0; i < db->colcount; i++)
    {
        switch (db->cols[i].type)
        {
            case 0:
            {
                int buff;
                db->file->read((char *)&buff, 4);
                vec.push_back(vdb::Value(buff));
            }; break;
            case 1:
            {
                double buff;
                db->file->read((char *)&buff, 8);
                vec.push_back(vdb::Value(buff)); // FIX DOUBLE() FIRST!!!
            }; break;
            case 2:
            {
                char buff;
                db->file->read(&buff, 1);
                vec.push_back(vdb::Value(buff));
            }; break;
            case 3:
            {
                char buff[32];
                db->file->read(buff, 32);
                vec.push_back(vdb::Value(buff, 32));
            }; break;
            case 4:
            {
                char buff[64];
                db->file->read(buff, 64);
                vec.push_back(vdb::Value(buff, 64));
            }; break;
        }
    }
    db->file->seekg(0);
}
void delete_record(meta *db, int line)
{
    if (line < 0 || line >= db->rowcount)
        return;

    int db_size = db->meta_size + (db->rowsize * db->rowcount);

    char *buffer = new char[db_size];

    db->file->read(buffer, db_size);

    db->file->close();
    db->file->open(db->file_name, std::ios::binary | std::ios::out | std::ios::in | std::ios::trunc);

    int part_1 = db->meta_size + (db->rowsize * line);
    int part_2 = db->rowsize * (db->rowcount + 1 - line);

    db->file->write(buffer, part_1);
    db->file->write(buffer + db->rowsize + part_1, part_2);

    delete[] buffer;

    db->file->seekp(3);
    --db->rowcount;
    db->file->write((char *)&db->rowcount, 2);
    db->file->seekp(0);
}
void clear_db(meta *db)
{
    char *buff = new char[db->meta_size];

    db->file->read(buff, db->meta_size);

    db->file->close();
    db->file->open(db->file_name, std::ios::binary | std::ios::out | std::ios::in | std::ios::trunc);

    db->file->write(buff, db->meta_size);
    db->file->seekp(3);
    db->rowcount = 0;
    db->file->write((char *)&db->rowcount, 2);
    db->file->seekp(0);
    delete[] buff;
}
