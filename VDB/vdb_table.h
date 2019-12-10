#pragma once

#ifndef VDB_TABLE_H_
#define VDB_TABLE_H_

#include <cstdint>
#include <string>
#include <fstream>
#include <variant>

#include "vdb_response.h"

namespace vdb
{

typedef struct
{
	uint8_t type;
	char name[32];
	uint8_t size;
} column__; // Name will be fixed... (And in open() function)

namespace
{
	struct Node
	{
		std::variant<vdb::Value, uint8_t, char> value;
		struct Node *right;
		struct Node *left;
		Node()
		{
			right = left = nullptr;
		}
	};
	bool is_match(vdb::Row &row, const Node *tree)
	{
		if (tree->value.index() == 0 || tree->value.index() == 1)
			return false;
		if (tree->value.index() == 2)
		{
			if ((std::get<char>(tree->value) != '&') && (std::get<char>(tree->value) != '|'))
			{
				switch (std::get<char>(tree->value))
				{
					case '=':
					{
						switch (row[tree->left->value.index() == 0 ? std::get<uint8_t>(tree->right->value) : std::get<uint8_t>(tree->left->value)].get_type())
						{
							case 0:
							{
								return (tree->left->value.index() == 0 ? static_cast<int>(std::get<vdb::Value>(tree->left->value)) : static_cast<int>(row[std::get<uint8_t>(tree->left->value)])) == (tree->right->value.index() == 0 ? static_cast<int>(std::get<vdb::Value>(tree->right->value)) : static_cast<int>(row[std::get<uint8_t>(tree->right->value)]));
							}; break;
							case 1:
							{
								return (tree->left->value.index() == 0 ? static_cast<double>(std::get<vdb::Value>(tree->left->value)) : static_cast<double>(row[std::get<uint8_t>(tree->left->value)])) == (tree->right->value.index() == 0 ? static_cast<double>(std::get<vdb::Value>(tree->right->value)) : static_cast<double>(row[std::get<uint8_t>(tree->right->value)]));
							}; break;
							case 2:
							{
								return (tree->left->value.index() == 0 ? static_cast<char>(std::get<vdb::Value>(tree->left->value)) : static_cast<char>(row[std::get<uint8_t>(tree->left->value)])) == (tree->right->value.index() == 0 ? static_cast<char>(std::get<vdb::Value>(tree->right->value)) : static_cast<char>(row[std::get<uint8_t>(tree->right->value)]));
							}; break;
							case 3:
							case 4:
							{
								// strcmp() returns 0 if strings are equals.
								return !static_cast<bool>(std::strcmp((tree->left->value.index() == 0 ? static_cast<char *>(std::get<vdb::Value>(tree->left->value)) : static_cast<char *>(row[std::get<uint8_t>(tree->left->value)])), (tree->right->value.index() == 0 ? static_cast<char *>(std::get<vdb::Value>(tree->right->value)) : static_cast<char *>(row[std::get<uint8_t>(tree->right->value)]))));
							}; break;
							default:
							{
								// Maybe I should write something here?
							}
						}
					}; break;
					case '!':
					{
						switch (row[tree->left->value.index() == 0 ? std::get<uint8_t>(tree->right->value) : std::get<uint8_t>(tree->left->value)].get_type())
						{
							case 0:
							{
								return (tree->left->value.index() == 0 ? static_cast<int>(std::get<vdb::Value>(tree->left->value)) : static_cast<int>(row[std::get<uint8_t>(tree->left->value)])) != (tree->right->value.index() == 0 ? static_cast<int>(std::get<vdb::Value>(tree->right->value)) : static_cast<int>(row[std::get<uint8_t>(tree->right->value)]));
							}; break;
							case 1:
							{
								return (tree->left->value.index() == 0 ? static_cast<double>(std::get<vdb::Value>(tree->left->value)) : static_cast<double>(row[std::get<uint8_t>(tree->left->value)])) != (tree->right->value.index() == 0 ? static_cast<double>(std::get<vdb::Value>(tree->right->value)) : static_cast<double>(row[std::get<uint8_t>(tree->right->value)]));
							}; break;
							case 2:
							{
								return (tree->left->value.index() == 0 ? static_cast<char>(std::get<vdb::Value>(tree->left->value)) : static_cast<char>(row[std::get<uint8_t>(tree->left->value)])) != (tree->right->value.index() == 0 ? static_cast<char>(std::get<vdb::Value>(tree->right->value)) : static_cast<char>(row[std::get<uint8_t>(tree->right->value)]));
							}; break;
							case 3:
							case 4:
							{
								// strcmp() returns 0 if strings are equals.
								return static_cast<bool>(std::strcmp((tree->left->value.index() == 0 ? static_cast<char *>(std::get<vdb::Value>(tree->left->value)) : static_cast<char *>(row[std::get<uint8_t>(tree->left->value)])), (tree->right->value.index() == 0 ? static_cast<char *>(std::get<vdb::Value>(tree->right->value)) : static_cast<char *>(row[std::get<uint8_t>(tree->right->value)]))));
							}; break;
							default:
							{
								// Maybe I should write something here?
							}
						}
					}; break;
					case '>':
					{
						switch (row[tree->left->value.index() == 0 ? std::get<uint8_t>(tree->right->value) : std::get<uint8_t>(tree->left->value)].get_type())
						{
							case 0:
							{
								return (tree->left->value.index() == 0 ? static_cast<int>(std::get<vdb::Value>(tree->left->value)) : static_cast<int>(row[std::get<uint8_t>(tree->left->value)])) > (tree->right->value.index() == 0 ? static_cast<int>(std::get<vdb::Value>(tree->right->value)) : static_cast<int>(row[std::get<uint8_t>(tree->right->value)]));
							}; break;
							case 1:
							{
								return (tree->left->value.index() == 0 ? static_cast<double>(std::get<vdb::Value>(tree->left->value)) : static_cast<double>(row[std::get<uint8_t>(tree->left->value)])) > (tree->right->value.index() == 0 ? static_cast<double>(std::get<vdb::Value>(tree->right->value)) : static_cast<double>(row[std::get<uint8_t>(tree->right->value)]));
							}; break;
							default:
							{
								// Maybe I should write something here?
							}
						}
					}; break;
					case '<':
					{
						switch (row[tree->left->value.index() == 0 ? std::get<uint8_t>(tree->right->value) : std::get<uint8_t>(tree->left->value)].get_type())
						{
							case 0:
							{
								return (tree->left->value.index() == 0 ? static_cast<int>(std::get<vdb::Value>(tree->left->value)) : static_cast<int>(row[std::get<uint8_t>(tree->left->value)])) < (tree->right->value.index() == 0 ? static_cast<int>(std::get<vdb::Value>(tree->right->value)) : static_cast<int>(row[std::get<uint8_t>(tree->right->value)]));
							}; break;
							case 1:
							{
								return (tree->left->value.index() == 0 ? static_cast<double>(std::get<vdb::Value>(tree->left->value)) : static_cast<double>(row[std::get<uint8_t>(tree->left->value)])) < (tree->right->value.index() == 0 ? static_cast<double>(std::get<vdb::Value>(tree->right->value)) : static_cast<double>(row[std::get<uint8_t>(tree->right->value)]));
							}; break;
							default:
							{
								// Maybe I should write something here?
							}
						}
					}; break;
					case 'g': // >=
					{
						switch (row[tree->left->value.index() == 0 ? std::get<uint8_t>(tree->right->value) : std::get<uint8_t>(tree->left->value)].get_type())
						{
							case 0:
							{
								return (tree->left->value.index() == 0 ? static_cast<int>(std::get<vdb::Value>(tree->left->value)) : static_cast<int>(row[std::get<uint8_t>(tree->left->value)])) >= (tree->right->value.index() == 0 ? static_cast<int>(std::get<vdb::Value>(tree->right->value)) : static_cast<int>(row[std::get<uint8_t>(tree->right->value)]));
							}; break;
							case 1:
							{
								return (tree->left->value.index() == 0 ? static_cast<double>(std::get<vdb::Value>(tree->left->value)) : static_cast<double>(row[std::get<uint8_t>(tree->left->value)])) >= (tree->right->value.index() == 0 ? static_cast<double>(std::get<vdb::Value>(tree->right->value)) : static_cast<double>(row[std::get<uint8_t>(tree->right->value)]));
							}; break;
							default:
							{
								// Maybe I should write something here?
							}
						}
					}; break;
					case 'l': // <=
					{
						switch (row[tree->left->value.index() == 0 ? std::get<uint8_t>(tree->right->value) : std::get<uint8_t>(tree->left->value)].get_type())
						{
							case 0:
							{
								return (tree->left->value.index() == 0 ? static_cast<int>(std::get<vdb::Value>(tree->left->value)) : static_cast<int>(row[std::get<uint8_t>(tree->left->value)])) <= (tree->right->value.index() == 0 ? static_cast<int>(std::get<vdb::Value>(tree->right->value)) : static_cast<int>(row[std::get<uint8_t>(tree->right->value)]));
							}; break;
							case 1:
							{
								return (tree->left->value.index() == 0 ? static_cast<double>(std::get<vdb::Value>(tree->left->value)) : static_cast<double>(row[std::get<uint8_t>(tree->left->value)])) <= (tree->right->value.index() == 0 ? static_cast<double>(std::get<vdb::Value>(tree->right->value)) : static_cast<double>(row[std::get<uint8_t>(tree->right->value)]));
							}; break;
							default:
							{
								// Maybe I should write something here?
							}
						}
					}; break;
					default:
						break;
				}
			}
		}

		bool op_1 = is_match(row, tree->left);
		bool op_2 = is_match(row, tree->right);

		if (std::get<char>(tree->value) == '|')
			return op_1 || op_2;
		else
			return op_1 && op_2;

	}
	//TODO INSIDE
	void set_tree(std::string str, Node *tree,  vdb::column__ *cols, const uint8_t colcount)
	{
		uint8_t nesting_level = 0;
		bool operations_left = false;

		// To remove wrapping brackets, for example (a > b), crucial for function to work
		// It should also distingiush something like (a == b) && (c < d) from wrapping brackets
		// UPD: Got to be improved. There're problems with 3 or above levels of nesting
		if (str[0] == '(' && str[str.length() - 1] == ')')
		{
			// If there's ')' braket, but '(' braket (open = false) doesn't accured, no action should be done
			bool open = false, error = false;
			// skip first and last character
			for (size_t i = 1; i < str.length() - 1; ++i)
			{
				if (str[i] == '(')
				{
					open = true;
				}
				else if (str[i] == ')')
				{
					if (!open)
					{
						error = true;
						break;
					}
					else
						open = false;
				}
			}

			if (!error)
				str = str.substr(1, str.length() - 2);
		}


		for (size_t i = 0; i < str.length(); ++i)
		{
			if (str[i] == '(')
			{
				++nesting_level;
			}
			else if (str[i] == ')')
			{
				--nesting_level;
			}
			else if (str[i] == '=' || str[i] == '!' || str[i] == '>' || str[i] == '<' || (str[i] == '>' && str[i + 1] == '=') || (str[i] == '<' && str[i + 1] == '='))
			{
				operations_left = true;
			}
			else if ((str[i] == '&' || str[i] == '|') && nesting_level == 0)
			{
				tree->value = str[i];
				tree->left = new Node();
				tree->right = new Node();
				set_tree(str.substr(0, i), tree->left, cols, colcount);
				set_tree(str.substr(i + 2, str.length() - i - 2), tree->right, cols, colcount);
				return;
			}
		}

		nesting_level = 0;

		// If there's no &&, || operations.
		if (operations_left)
		{
			for (size_t i = 0; i < str.length(); ++i)
			{
				if (str[i] == '(')
				{
					++nesting_level;
				}
				else if (str[i] == ')')
				{
					--nesting_level;
				}
				else if ((str[i] == '=' || str[i] == '!' || str[i] == '>' || str[i] == '<' || (str[i] == '>' && str[i+1] == '=') || (str[i] == '<' && str[i + 1] == '=')) && nesting_level == 0)
				{
					uint8_t is_double_char = 0; // to get correct substring when operator is two character long (<= and >=)
					if(str[i] == '>' && str[i + 1] == '=')
					{
						tree->value = 'g';
						is_double_char = 1;
					}
					else if (str[i] == '<' && str[i + 1] == '=')
					{
						tree->value = 'l';
						is_double_char = 1;
					}
					else
						tree->value = str[i];
					
					tree->left = new Node();
					tree->right = new Node();
					set_tree(str.substr(0, i), tree->left, cols, colcount);
					set_tree(str.substr(i + 1 + is_double_char, str.length() - i - 1 - is_double_char), tree->right, cols, colcount);
					return;
				}
			}
		}
		else
		{
			if (str[0] == '`') // column name
			{
				str = str.substr(1, str.length() - 2);

				for (uint8_t i = 0; i < colcount; i++)
				{
					if(std::strcmp(str.c_str(), cols[i].name) == 0)
					{
						tree->value = i;
						return;
					}
				}

				std::cout << "poshol v pizdu!" << std::endl;
				// TODO:
				// DO SOMETHING WHEN THERE'S NO SUCH COLUMNS.
				// Maybe use the value of 255 to indicate that there's no columns
			}
			else if (str[0] == '\'') // char value
			{
				vdb::Value val(str[1]);
				tree->value = val;
			}
			else if (str[0] == '\"') // string value
			{
				vdb::Value val(str.substr(1, str.length() - 2).c_str());
				tree->value = val;
			}
			else // int or double value
			{
				// TODO: try/catch to catch bad argument exception if any
				vdb::Value val(std::stod(str));
				tree->value = val;
			}
		}
	}
	void destroy_tree(struct Node *node)
	{
		if (node != nullptr)
		{
			destroy_tree(node->left);
			destroy_tree(node->right);
			delete node;
		}
	}
}

bool create_db(const std::string &desc);
bool create_db(const char *db_path, vdb::column *cols, uint8_t colcount);

class Table
{
private:
	uint16_t meta_size;
	uint8_t colcount;
	uint16_t rowcount;
	uint16_t rowsize;
	column__ *cols;
	std::fstream file;
	std::string file_name;

	bool opened;

public:

	Table();
	Table(const Table &table);
	Table &operator=(const Table &table);
	~Table();

	bool open(const std::string &name);
	bool is_open();

	void close();

	// ONLY FOR DEBUG PURPOSES
	void print_col_names()
	{
		for (uint8_t i = 0; i < colcount; i++)
		{
			std::cout << cols[i].name << "(" << int(cols[i].type) << ")" << "\t";
		}
		std::cout << std::endl;
	}


	// there will be more overloaded functions
	void insert_into(Value *vals);
	void insert_into(Row &row);

	Response select_where(std::string &condition);
	Response select_where(const char *condition);

	Response select_all();
	void remove();
	void remove_line(size_t line);
	void clear();
};


}


#endif // !VDB_TABLE_H_
