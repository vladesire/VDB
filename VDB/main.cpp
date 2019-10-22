#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstring>

void buff_write(char *buff, char *source)
{
    int some_kind_of_variable = 4;
    for (int i = 0; i < some_kind_of_variable; i++)
        buff[i] = source[i];
}

void from_buff(int &dest, char *buff)
{
    dest = 0;

    for (int i = 0; i < 4; ++i)
    {
        dest |= (int(unsigned char((buff[i]))) << (i * 8));
    }

}

class vdb_value
{
private:
    char *buff;
    uint8_t type;
    int int_val() const
    {
        int a;
        memcpy((char *)&a, buff, 4);
        return a;
    }
    double double_val() const
    {
        double a;
        memcpy((char *)&a, buff, 8);
        return a;
    }
public:
    vdb_value(const vdb_value &p2)
    {
        type = p2.type;
        int size = (type == 0) ? 4 : (type == 1 ? 8 : (type == 2 ? 1 : type == 3 ? 32 : 64));
        buff = new char[size];
        memcpy(buff, p2.buff, size);
    };
    vdb_value &operator=(vdb_value other)
    {
        type = other.type;
        int size = (type == 0) ? 4 : (type == 1 ? 8 : (type == 2 ? 1 : type == 3 ? 32 : 64));
        buff = new char[size];
        memcpy(buff, other.buff, size);
    };

    vdb_value(int val)
    {
        type = 0;
        buff = new char[4];
        for (int i = 0; i < 4; i++)
            buff[i] = ((char *)(&val))[i];
    }
    vdb_value(double val)
    {
        type = 1;
        buff = new char[8];
        for (int i = 0; i < 8; i++)
            buff[i] = ((char *)(&val))[i];
    }
    vdb_value(char val)
    {
        type = 2;
        buff = new char[1];
        *buff = val;
    }
    vdb_value(char *val, int size)
    {
        type = (size == 32) ? 3 : 4;
        buff = new char[size];
        memcpy(buff, val, size);
    }
    operator int() const
    {
        return int_val();
    }
    operator double() const
    {
        return double_val();
    }

    operator char() const
    {
        return buff[0];
    }
    operator char *() const
    {
        return buff;
    }
    std::string to_string()
    {
        switch (type)
        {
            case 0:
                return std::to_string(int_val());
            case 1:
                return std::to_string(double_val());
            case 2:
                return std::string(1, buff[0]);
            case 3:
            case 4:
                return std::string(buff);
        }
    }
    ~vdb_value()
    {
        delete[] buff;
    }
};


enum coltype
{
    INT = 0, DOUBLE, CHAR, STR32, STR64
};

typedef struct
{
    uint8_t type;
    char name[32];
} column;

typedef std::vector<vdb_value> vrecord;

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

/*
    string description structure:
    `DB_PATH_WITH_DB_NAME` COLCOUNT COLTYPE_1 `COLNAME_1` ... COLTYPE_N `COLNAME_N`
*/

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

void show_meta(meta *db)
{
    printf("db->size: %d \n", db->meta_size);
    printf("db->colcount: %d \n", db->colcount);
    printf("db->rowcount: %d \n", db->rowcount);
    printf("db->rowsize: %d \n", db->rowsize);
    for (int i = 0; i < db->colcount; i++)
    {
        printf("col[%d].name: %s \n", i, db->cols[i].name);
        printf("col[%d].type: %d \n", i, db->cols[i].type);
    }
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
                vec.push_back(vdb_value(buff));
            }; break;
            case 1:
            {
                double buff;
                db->file->read((char *)&buff, 8);
                vec.push_back(vdb_value(buff)); // FIX DOUBLE() FIRST!!!
            }; break;
            case 2:
            {
                char buff;
                db->file->read(&buff, 1);
                vec.push_back(vdb_value(buff));
            }; break;
            case 3:
            {
                char buff[32];
                db->file->read(buff, 32);
                vec.push_back(vdb_value(buff, 32));
            }; break;
            case 4:
            {
                char buff[64];
                db->file->read(buff, 64);
                vec.push_back(vdb_value(buff, 64));
            }; break;
        }
    }
    db->file->seekg(0);
}

void show_record(meta *db, vrecord &vec)
{
    for (int i = 0; i < db->colcount; i++)
        std::cout << vec[i].to_string() << "\t";
    std::cout << std::endl;

}

void show_db(meta *db)
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
        show_record(db, rec);
    }
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


int main()
{/*
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

    //delete_record(db, 5);

    show_db(db);

    clear_db(db);

    make_record(db, "5 32 `Dasha`");
    make_record(db, "2 34 `Vladislav`");
    make_record(db, "7 62 `Gusek`");


    show_db(db);

    delete_record(db, 2);

    show_db(db);

    close_db(db);
    */

    
    int a = 12345, b;
    char buff[4];

    memcpy(buff, (char *)&a, 4);

    memcpy((char *)&b, buff, 4);

    std::cout << b;

    std::cin.get();
    return 0;
}
