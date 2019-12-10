#include <iostream>
#include "vdb_api.h"
#include <vector>

enum coltype
{
    INT = 0, DOUBLE, CHAR, STR32, STR64 // Null = 0?
};

/*
    string description structure:
    `DB_PATH_WITH_DB_NAME` COLCOUNT COLTYPE_1 `COLNAME_1` ... COLTYPE_N `COLNAME_N`
*/

void print_response(vdb::Response &response)
{
	for (size_t i = 0; i < response.size(); i++)
	{
		for (size_t j = 0; j < response[i].get_size(); j++)
		{
			std::cout << response[i][j].to_string() << "\t";
		}
		std::cout << std::endl;
	}
}

void print_rows(vdb::Row *row, size_t size)
{
	for (size_t i = 0; i < size; ++i)
	{
		for (size_t j = 0; j < row[i].get_size(); ++j)
		{
			std::cout << row[i][j].to_string() << " ";
		}
		std::cout << std::endl;
	}
}

/*void foo()
{
	using namespace vdb;

	Row row_1('f', "azak");
	Row row_2("alala", 5.4);
	Row row_3(12, 'h');
	Row row_4(8841, 1488);

	Row rows[4];

	rows[0] = row_1;
	rows[1] = row_2;
	rows[2] = row_3;
	rows[3] = row_4;

	Response resp(rows, 4);

	print_response(resp);
}*/

//#include <variant>

/*
operations:
0: == (=)
1: != (!)
2: >
3: <
4: >= (g)
5: <= (l)
*/

/*struct Node
{
	std::variant<vdb::Value, uint8_t, char> value;
	struct Node *right;
	struct Node *left;
	Node()
	{
		right = left = nullptr;
	}
};
void destroy_tree(struct Node *node)
{
	if (node != nullptr)
	{
		destroy_tree(node->left);
		destroy_tree(node->right);
		delete node;
	}
}

bool is_match(vdb::Row &row, const Node *tree);

void set_tree(std::string str, Node *tree)
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
		else if (str[i] == '=' || str[i] == '!' || str[i] == '>' || str[i] == '<' || str[i] == 'g' || str[i] == 'l')
		{
			operations_left = true;
		}
		else if ((str[i] == '&' || str[i] == '|') && nesting_level == 0)
		{
			tree->value = str[i];
			tree->left = new Node();
			tree->right = new Node();
			set_tree(str.substr(0, i), tree->left);
			set_tree(str.substr(i + 2, str.length() - i - 1), tree->right);
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
			else if ((str[i] == '=' || str[i] == '!' || str[i] == '>' || str[i] == '<' || str[i] == 'g' || str[i] == 'l') && nesting_level == 0)
			{
				tree->value = str[i];
				tree->left = new Node();
				tree->right = new Node();
				set_tree(str.substr(0, i), tree->left);
				set_tree(str.substr(i + 2, str.length() - i - 1), tree->right);
			}
		}
	}
	else
	{
		if (str[0] == '`') // column name
		{
			str = str.substr(1, str.length() - 2);

		// KOSTYL:

			//table.

			// TODO: find column number from its name
//			tree->value = column_number;
		}
		else if(str[0] == '\'') // char value
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
			vdb::Value val(std::stod(str));
			tree->value = val;
		}
	}
}

// FLAWED DESIGN
// get_response can be inserted into where(), there's no need to call another function
void get_response(vdb::Response &resp, Node *tree)
{

	uint16_t match_count = 0;
	vdb::Row *match_indexes = new vdb::Row[resp.size()]; // I SHOULD STORE ONLY POINTERS TO THE ROWS, NOT VALUES ITSELF (OR NOT???)

	for (uint16_t i = 0; i < resp.size(); i++)
		if (is_match(resp[i], tree))
			match_indexes[match_count++] = resp[i]; // sizeof(vdb::Row) > sizeof(vdb::Row *), but should I define operator= for pointers copy?

	vdb::Response new_response(match_indexes, match_count);

	resp = new_response;

	delete[] match_indexes;
}

vdb::Response where_query_processor(std::string &str)
{
	// All blanks must be truncated
	for (auto it = str.begin(); it != str.end(); ++it)
		if (*it == ' ')
			str.erase(it--);

	Node *root = new Node;	

	vdb::Response resp; // REPLACE IT

	set_tree(str, root);

	uint16_t match_count = 0;
	vdb::Row *match_indexes = new vdb::Row[resp.size()]; // I SHOULD STORE ONLY POINTERS TO THE ROWS, NOT VALUES ITSELF (OR NOT???)

	for (uint16_t i = 0; i < resp.size(); i++)
		if (is_match(resp[i], root))
			match_indexes[match_count++] = resp[i]; // sizeof(vdb::Row) > sizeof(vdb::Row *), but should I define operator= for pointers copy?

	vdb::Response new_response(match_indexes, match_count);

	resp = new_response;

	delete[] match_indexes;
	destroy_tree(root);

	return resp;
}

vdb::Response where_query_processor(const char *str)
{
	std::string query(str);
	return where_query_processor(query);
}


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
				case 'g':
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
				case 'l':
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

}*/





int main()
{
	vdb::Table table;

	table.open("vladesire");

	//table.clear();

	/*vdb::Row row_1(1, 50, "String one");
	vdb::Row row_2(2, 5, "Still what to fight?");
	vdb::Row row_3(3, 45, "Hellyeah");
	vdb::Row row_4(4, 10, "Fucking A");
	vdb::Row row_5(5, 40, "String two");
	vdb::Row row_6(6, 15, "There's still a reason ...");
	vdb::Row row_7(7, 35, "Holy shit");
	vdb::Row row_8(8, 20, "Really?");
	vdb::Row row_9(9, 30, "Vladesire is a Power");
	vdb::Row row_10(10, 25, "This is not a victory.");

	table.insert_into(row_1);
	table.insert_into(row_2);
	table.insert_into(row_3);
	table.insert_into(row_4);
	table.insert_into(row_5);
	table.insert_into(row_6);
	table.insert_into(row_7);
	table.insert_into(row_8);
	table.insert_into(row_9);
	table.insert_into(row_10);*/

	//table.remove_line(2); // PROBLEM <---------------------------------------

	//vdb::Response all = table.select_all();
	vdb::Response r = table.select_where("(`height` >= 15 && `height` <= 30) || `name` = \"Hellyeah\"");

	table.print_col_names();

	print_response(r);

	table.close();

	//vdb::Response resp = where_query_processor("var = 23 || lib = `vladesire`");
	//print_response(resp);

	/*Node *root = new Node();

	root->value = '&';

	root->right = new Node();
	root->left = new Node();
	root->left->left = new Node();
	root->left->right = new Node();
	root->right->left = new Node();
	root->right->right = new Node();

	vdb::Value val_1(7);
	vdb::Value val_2("Some string");

	root->left->value = '>';
	root->left->left->value = (uint8_t)0;
	root->left->right->value = val_1;

	root->right->value = '=';
	root->right->left->value = (uint8_t)1;
	root->right->right->value = val_2;

	vdb::Row row(15, "Some string", 53, "fasdfas", '3', 45);

	if (is_match(row, root))
		std::cout << "True";
	else
		std::cout << "False";


	destroy_tree(root);*/




    std::cin.get();
    return 0;
}