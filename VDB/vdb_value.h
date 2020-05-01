#ifndef VDB_VALUE_H
#define VDB_VALUE_H

#include <variant>
#include <string>

namespace vdb
{

class Value
{
private:
	std::variant<int, double, char, char *> val;

public:
	friend void swap(Value &val1, Value &val2);
	friend std::ostream &operator<<(std::ostream &os, Value val);

	Value() = default;
	Value(const Value &value);
	Value(Value &&value) noexcept;

	Value &operator=(Value value) noexcept;

	template <class T>
	Value(T value)
	{
		reset(); //if constructor is used for type conversion
		val = value;
	}

	template <> // it's identical to const char, but template generates new code for it
	Value(char *str) : Value(str, false) { }

	Value(const char *str, bool = false);

	
	~Value()
	{
		reset();
	}
	constexpr uint8_t type() const
	{
		return val.index();
	}

	void reset();

	// I decided to add implicit int-double conversion as it can be useful in some cases
	operator int() const
	{
		return val.index() == 1 ? static_cast<int>(std::get<double>(val)) : std::get<int>(val);
	}
	operator double() const
	{
		return val.index() == 0 ? static_cast<double>(std::get<int>(val)) : std::get<double>(val);
	}
	operator char() const
	{
		return std::get<char>(val);
	}
	operator char *() const
	{
		return std::get<char *>(val);
	}
	const char *cptr();

	bool operator>(const Value &val1)
	{
		switch (val.index())
		{
		case 0:
			return std::get<0>(val) > (int)(val1);
		case 1:
			return std::get<1>(val) > (double)(val1);
		case 2:
		case 3:
			throw std::exception("Comparision (>, >=, <, <=) of strings and chars is not supported");
		}
	}
	bool operator<(const Value &val1)
	{
		switch (val.index())
		{
			case 0:
				return std::get<0>(val) < (int)(val1);
			case 1:
				return std::get<1>(val) < (double)(val1);
			case 2:
			case 3:
				throw std::exception("Comparision (>, >=, <, <=) of strings and chars is not supported");
		}
	}
	bool operator>=(const Value &val1)
	{
		switch (val.index())
		{
			case 0:
				return std::get<0>(val) >= (int)(val1);
			case 1:
				return std::get<1>(val) >= (double)(val1);
			case 2:
			case 3:
				throw std::exception("Comparision (>, >=, <, <=) of strings and chars is not supported");
		}
	}
	bool operator<=(const Value &val1)
	{
		switch (val.index())
		{
			case 0:
				return std::get<0>(val) <= (int)(val1);
			case 1:
				return std::get<1>(val) <= (double)(val1);
			case 2:
			case 3:
				throw std::exception("Comparision (>, >=, <, <=) of strings and chars is not supported");
		}
	}
	bool operator==(const Value &val1)
	{
		switch (val.index())
		{
			case 0:
				return std::get<0>(val) == (int)(val1);
			case 1:
				return std::get<1>(val) == (double)(val1);
			case 2:
				return std::get<2>(val) == (char)(val1);
			case 3:
				return !strcmp(std::get<3>(val), (char *)(val1));
		}
	}
	bool operator!=(const Value &val1)
	{
		switch (val.index())
		{
			case 0:
				return std::get<0>(val) != (int)(val1);
			case 1:
				return std::get<1>(val) != (double)(val1);
			case 2:
				return std::get<2>(val) != (char)(val1);
			case 3:
				return strcmp(std::get<3>(val), (char *)(val1));
		}
		
	}


	std::string to_string();
};

template <class T>
T get(const Value &val)
{
	return static_cast<T>(val);
};

}
#endif // !Value_H
