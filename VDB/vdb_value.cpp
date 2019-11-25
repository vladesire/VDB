#include "vdb_value.h"

int vdb::Value::int_val() const
{
	int a;
	memcpy(&a, buff, 4);
	return a;
}
double vdb::Value::double_val() const
{
	double a;
	memcpy(&a, buff, 8);
	return a;
}

vdb::Value::Value(const vdb::Value &val)
{
	type = val.type;
	int size = (type == 0) ? 4 : (type == 1 ? 8 : (type == 2 ? 1 : type == 3 ? 32 : 64));
	buff = new char[size];
	memcpy(buff, val.buff, size);
};
void vdb::Value::operator=(vdb::Value &val)
{
	type = val.type;
	int size = (type == 0) ? 4 : (type == 1 ? 8 : (type == 2 ? 1 : type == 3 ? 32 : 64));

	delete[] buff; //if buff == nullptr, there'll be no effect

	buff = new char[size];
	memcpy(buff, val.buff, size);
};
vdb::Value::Value()
{
}
vdb::Value::Value(int val)
{
	type = 0;
	buff = new char[4];
	memcpy(buff, &val, 4);
}
vdb::Value::Value(double val)
{
	type = 1;
	buff = new char[8];
	memcpy(buff, &val, 8);
}
vdb::Value::Value(char val)
{
	type = 2;
	buff = new char[1];
	*buff = val;
}
vdb::Value::Value(char *val, int size)
{
	type = (size == 32) ? 3 : 4;
	buff = new char[size];
	memcpy(buff, val, size);
}
vdb::Value::Value(const char *val)
{
	size_t size = strlen(val);
	type = (size <= 32) ? 3 : 4;
	buff = new char[type == 3 ? 32 : 64];
	memcpy(buff, val, type == 3 ? 32 : 64);
}
vdb::Value::operator int() const
{
	return int_val();
}
vdb::Value::operator double() const
{
	return double_val();
}
vdb::Value::operator char() const
{
	return buff[0];
}
vdb::Value::operator char *() const
{
	return buff;
}
std::string vdb::Value::to_string()
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
	return std::string("");
}
vdb::Value::~Value()
{
	delete[] buff;
}