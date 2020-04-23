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

	template <>
	Value(const char *str)
	{
		reset(); //if constructor is used for type conversion

		auto size = strlen(str);
		bool bit = false;

		if (size > 64)
			bit = true;

		val = new char[bit ? 64 : size + 1];
		memcpy(std::get<char *>(val), str, bit ? 64 : size + 1);
		std::get<char *>(val)[bit ? 63 : size] = '\0';
	}
	~Value()
	{
		reset();
	}

	uint8_t get_type() const
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

	std::string to_string();
};

template <class T>
T get(Value &val)
{
	return static_cast<T>(val);
};


}
#endif // !Value_H
