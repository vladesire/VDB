#include "vdb_table.h"

namespace 
{ 
	static inline void ltrim(std::string &s)
	{
		s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int ch)
		{
			return !std::isspace(ch);
		}));
	}
	static inline void rtrim(std::string &s)
	{
		s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch)
		{
			return !std::isspace(ch);
		}).base(), s.end());
	}
	void unescape(std::string &str) // \" -> ", \' -> ', etc.
	{
		for (auto it = str.begin(); it != str.end(); ++it)
		{
			if (*it == '\\')
			{
				str.erase(it);
			}
		}
	}
	void remove_spaces(std::string &str)  // all not single spaces, leading and ending spaces are deleted
	{
		ltrim(str);
		rtrim(str);

		for (auto it = str.begin(); it != str.end(); ++it)
		{
			if (*it == '`')
			{
				while ((*(++it) != '`') && it != str.end())
					if (*it == '\\' && (it + 1) != str.end() && (it + 2) != str.end())
						++it;
			}
			else if (*it == '\"')
			{
				while ((*(++it) != '\"') && it != str.end())
					if (*it == '\\' && (it + 1) != str.end() && (it + 2) != str.end())
						++it;
			}
			else if (*it == ' ')
			{
				if (*(it + 1) == ' ')
					str.erase(it--);
			}
		}

	}
	std::string next_token(std::string &source, const char *delim)
	{
		size_t pos = source.find_first_of(delim);

		if (pos == std::string::npos)
			return source;

		// To skip "pre" delimeters
		size_t skip = 0;
		if (pos == 0)
		{
			skip = source.find_first_not_of(delim);
			pos = source.find_first_of(delim, skip);
		}

		std::string temp;
		temp = source.substr(skip, pos - skip);
		source = source.substr(pos + 1);
		return temp;
	}

	// This is query processor stuff -------------------------->
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
				if ((tree->left->value.index() == 0 ? std::get<uint8_t>(tree->right->value) : std::get<uint8_t>(tree->left->value)) == 0xFF)
					return false; // return false if there's no column with such name (look set_tree()), 0xFF is indicator.

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
	void destroy_tree(struct Node *node)
	{
		if (node != nullptr)
		{
			destroy_tree(node->left);
			destroy_tree(node->right);
			delete node;
		}
	}
	//TODO INSIDE
	void set_tree(std::string str, Node *tree, const vdb::Table &table)
	{
		uint8_t nesting_level = 0;
		bool operations_left = false;

		// To remove wrapping brackets, for example (a > b), crucial for function to work
		// It should also distingiush something like (a == b) && (c < d) from wrapping brackets
		// TODO: There're problems with 3 or above levels of nesting <-------------------------------------------------
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
			else if (str[i] == '`')
			{
				while (str[++i] != '`')
					if (str[i] == '\\' && (i + 1) < str.length() && (i + 2) != str.length())
						++i;
			}
			else if (str[i] == '\"')
			{
				while (str[++i] != '\"')
					if (str[i] == '\\' && (i + 1) < str.length() && (i + 2) != str.length())
						++i;
			}
			else if ((str[i] == '=' && str[i + 1] == '=') || (str[i] == '!' && str[i + 1] == '=') || str[i] == '>' || str[i] == '<' || (str[i] == '>' && str[i + 1] == '=') || (str[i] == '<' && str[i + 1] == '='))
			{
				operations_left = true;
			}
			else if ((str[i] == '&' || str[i] == '|') && nesting_level == 0)
			{
				tree->value = str[i];
				tree->left = new Node();
				tree->right = new Node();
				set_tree(str.substr(0, i), tree->left, table);
				set_tree(str.substr(i + 2, str.length() - i - 2), tree->right, table);
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
				else if (str[i] == '`')
				{
					while (str[++i] != '`')
						if (str[i] == '\\' && (i + 1) < str.length() && (i + 2) != str.length())
							++i;
				}
				else if (str[i] == '\"')
				{
					while (str[++i] != '\"')
						if (str[i] == '\\' && (i + 1) < str.length() && (i + 2) != str.length())
							++i;
				}
				else if (((str[i] == '=' && str[i + 1] == '=') || (str[i] == '!' && str[i + 1] == '=') || str[i] == '>' || str[i] == '<' || (str[i] == '>' && str[i + 1] == '=') || (str[i] == '<' && str[i + 1] == '=')) && nesting_level == 0)
				{
					uint8_t is_double_char = 0; // to get correct substring when operator is two character long (<= and >=)
					if (str[i] == '>' && str[i + 1] == '=')
					{
						tree->value = 'g';
						is_double_char = 1;
					}
					else if (str[i] == '<' && str[i + 1] == '=')
					{
						tree->value = 'l';
						is_double_char = 1;
					}
					else if (str[i] == '!' && str[i + 1] == '=')
					{
						tree->value = '!';
						is_double_char = 1;
					}
					else if (str[i] == '=' && str[i + 1] == '=')
					{
						tree->value = '=';
						is_double_char = 1;
					}
					else
						tree->value = str[i];

					tree->left = new Node();
					tree->right = new Node();
					set_tree(str.substr(0, i), tree->left, table);
					set_tree(str.substr(i + 1 + is_double_char, str.length() - i - 1 - is_double_char), tree->right, table);
					return;
				}
			}
		}
		else
		{
			if (str[0] == '`') // column name
			{
				str = str.substr(1, str.length() - 2);

				unescape(str);

				for (uint8_t i = 0; i < table.get_colcount(); i++)
				{
					if (table.get_col_name(i) == str)
					{
						tree->value = i;
						return;
					}
				}

				tree->value = uint8_t(0xFF); // To indicate that there's no column with such name
			}
			else if (str[0] == '\'') // char value
			{
				vdb::Value val(str[1]);
				tree->value = val;
			}
			else if (str[0] == '\"') // string value
			{
				str = str.substr(1, str.length() - 2);
				unescape(str);
				vdb::Value val(str.c_str());
				tree->value = val;
			}
			else // int or double value
			{
				try
				{
					vdb::Value val(std::stod(str));
					tree->value = val;
				}
				catch (const std::exception &)
				{
					std::cout << "I TRUSTED YOU!!!!!" << std::endl;
					vdb::Value val(0);
					tree->value = val;
				}

			}
		}
	}
	// This is query processor stuff -------------------------->

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
	// Should I delete print_rows?
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
	void print_table_colnames(const vdb::Table table)
	{
		for (size_t i = 0; i < table.get_colcount(); ++i)
		{
			std::cout << table.get_col_name(i) << "\t";
		}
		std::cout << std::endl;
	}
}
bool vdb::create_db(std::string &desc) //check if desc has correct syntax
{
	std::string file_name;
	std::fstream file;

	remove_spaces(desc);

	// pos1 ... pos2
	size_t pos1;
	size_t pos2 = 0;
	while (desc[++pos2] != '`') // to skip all escaped `
	{
		if (desc[pos2] == '\\')
			++pos2;
	}

	// db path with name
	file_name = desc.substr(1, pos2 - 1);

	unescape(file_name); // \` -> `, \\ -> \

	if (file_name.substr(file_name.length() - 4, 4) != ".vdb")
		file_name.append(".vdb");

	file.open(file_name, std::ios::binary | std::ios::out);

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
		pos2 = pos1 - 1;
		//pos2 = desc.find_first_of('`', pos1);
		while (desc[++pos2] != '`') // to skip all escaped `
		{
			if (desc[pos2] == '\\')
				++pos2;
		}
		name = desc.substr(pos1, pos2 - pos1);
		unescape(name);
		file.write(name.c_str(), 32);
		++pos2;
	}

	// write rowsize
	file.write((char *)&rowsize, 2);

	file.close();

	return true;
}
bool vdb::create_db(const char *desc)
{
	std::string str(desc);
	return create_db(str);
}
bool vdb::create_db(const char *db_path, vdb::column *cols, uint8_t colcount)
{
	std::fstream file;
	file.open(db_path, std::ios::binary | std::ios::out);

	if (!file.is_open())
		return false;

	uint16_t two_bytes;

	//meta size
	two_bytes = 7 + (33 * colcount);

	file.write((char *)&two_bytes, 2);
	file.write((char *)&colcount, 1);

	two_bytes = 0;
	// number of rows (= 0)
	file.write((char *)&two_bytes, 2);

	int colsize[] = {4, 8, 1, 32, 64}; // int, double, char, str32, str64
	uint16_t rowsize = 0;

	for (int i = 0; i < colcount; i++)
	{
		file.write((char *)&cols[i].type, 1);
		file.write((char *)&cols[i].name, 32);
		rowsize += colsize[cols[i].type];
	}

	// write rowsize
	file.write((char *)&rowsize, 2);

	file.close();

	return true;
}

int vdb::query_cout(std::string query)
{

	ltrim(query);

	if (query[0] != '`')
	{
		std::string substr = next_token(query, " ");

		if (substr == "create")
		{
			substr = next_token(query, " ");

			if (substr == "table")
			{
				vdb::create_db(query);
				return 0;
			}
			else
				return 2;
		}
		else if (substr == "cls")
		{
			system("cls");
			return 0;
		}
		else if (substr == "exit")
			return 1812;
		else
			return 2;
	}

	size_t i = 0;
	while (query[++i] != '`')
		if (query[i] == '\\' && (i + 1) < query.length() && (i + 2) != query.length())
			++i;

	std::string table_name = query.substr(1, i - 1);

	query = query.substr(i + 2);

	unescape(table_name);

	vdb::Table table;
	table.open(table_name);

	if (!table.is_open())
		return -1;

	std::string substr = next_token(query, " ");

	if (substr == "select_all")
	{
		vdb::Response resp = table.select_all();
		print_table_colnames(table);
		print_response(resp);
	}
	else if (substr == "insert_into")
	{
		table.insert_into(query);
	}
	else if (substr == "select_where")
	{
		vdb::Response resp = table.select_where(query);
		print_table_colnames(table);
		print_response(resp);
	}
	else if (substr == "clear")
	{
		table.clear();
	}
	else if (substr == "remove_line")
	{
		try
		{
			size_t line = std::stoi(query);
			table.remove_line(line);
		}
		catch (const std::exception &)
		{
			std::cout << "Fuck you, asshole!";
		}
	}
	else if (substr == "delete")
	{
		table.close();
		std::string query("del ");
		query.append(table_name);
		if (table_name.substr(table_name.length() - 4, 4) != ".vdb")
			query.append(".vdb");
		std::system(query.c_str());
		return 0;
	}

	table.close();
	return 0;
}



// Class managment
vdb::Table::Table()
{
	opened = false;
}
vdb::Table::Table(const Table &table)
{
	if (!table.opened)
	{
		opened = false;
	}
	else
	{
		meta_size = table.meta_size;
		colcount = table.colcount;
		rowcount = table.rowcount;
		rowsize = table.rowsize;

		cols = new vdb::column[colcount];

		for (size_t i = 0; i < colcount; ++i)
		{
			strcpy(cols[i].name, table.cols[i].name);
			cols[i].type = table.cols[i].type;
			cols[i].size = table.cols[i].size;
		}

		file_name = table.file_name;

		if (file_name.substr(file_name.length() - 4, 4) != ".vdb")
			file_name.append(".vdb");

		file.open(file_name, std::ios::binary | std::ios::in | std::ios::out);

		opened = true;
	}
}
vdb::Table &vdb::Table::operator=(const vdb::Table &table)
{
	if (this == &table)
		return *this;
	if (!table.opened)
	{	
		opened = false;
	}
	else
	{
		meta_size = table.meta_size;
		colcount = table.colcount;
		rowcount = table.rowcount;
		rowsize = table.rowsize;

		cols = new vdb::column[colcount];

		for (size_t i = 0; i < colcount; ++i)
		{
			strcpy(cols[i].name, table.cols[i].name);
			cols[i].type = table.cols[i].type;
			cols[i].size = table.cols[i].size;
		}

		file_name = table.file_name;

		if (file_name.substr(file_name.length() - 4, 4) != ".vdb")
			file_name.append(".vdb");

		file.open(file_name, std::ios::binary | std::ios::in | std::ios::out);

		opened = true;
	}
	return *this;
}
vdb::Table::~Table()
{
	if (opened)
	{ 
		file.close();
		delete[] cols;
	}
}


// File managment
bool vdb::Table::open(const std::string &name)
{
	file_name = name;

	if (name != ".settings" && name.substr(name.length() - 4, 4) != ".vdb")
		file_name.append(".vdb");

	file.open(file_name, std::ios::binary | std::ios::in | std::ios::out);

	if (!file.is_open())
		return false;

	file.read((char *)&meta_size, 2);
	file.read((char *)&colcount, 1);
	file.read((char *)&rowcount, 2);

	cols = new vdb::column[colcount];

	for (int i = 0; i < colcount; i++)
	{
		file.read((char *)&(cols[i].type), 1);
		cols[i].size = (cols[i].type == 0) ? 4 : (cols[i].type == 1 ? 8 : (cols[i].type == 2 ? 1 : cols[i].type == 3 ? 32 : 64));
		file.read((char *)&(cols[i].name), 32);
	}

	file.read((char *)&rowsize, 2);

	file.seekg(0);

	opened = true;
	return true;
}
void vdb::Table::close()
{
	if (opened)
	{
		file.close();
		delete[] cols;
		opened = false;
	}
}
vdb::Response vdb::Table::vdb_query(std::string query)
{
	// IN DEVELOPMENT
	return vdb::Response();
}


// CRUD operations:

// Create
void vdb::Table::insert_into(vdb::Value *vals)
{
	// TODO: check if vals is valid (colcount) + think about auto-value in row (like null by default or autoincrement)
	file.seekp(0, std::ios::end);

	for (int i = 0; i < colcount; i++)
	{
		if (vals[i].get_type() != cols[i].type)
		{
			// Null value must be set if its possible (not_null = false)

			char a = 0;

			for (int i = 0; i < cols[i].size; ++i)
				file.write(&a, 1);
		}
		switch (cols[i].type)
		{
			// (char *)vals[i] will return pointer to vdb::Value inner buffer that contain the value
			case 0:
				file.write((char *)vals[i], 4); break;
			case 1:
				file.write((char *)vals[i], 8); break;
			case 2:
				file.write((char *)vals[i], 1); break;
			case 3:
				file.write((char *)vals[i], 32); break;
			case 4:
				file.write((char *)vals[i], 64); break;
		}
	}
	file.seekp(3);
	++rowcount;
	file.write((char *)&rowcount, 2);
	file.seekp(0);
}
void vdb::Table::insert_into(vdb::Row &row)
{
	// TODO: check if vals is valid (colcount) + think about auto-value in row (like null by default or autoincrement)
	file.seekp(0, std::ios::end);

	for (int i = 0; i < colcount; i++)
	{
		if (row[i].get_type() != cols[i].type)
		{
			// Null value must be set if its possible (not_null = false)

			char a = 0;

			for (int k = 0; k < cols[i].size; ++k)
				file.write(&a, 1);

			continue;
		}
		switch (cols[i].type)
		{
			// (char *)vals[i] will return pointer to vdb::Value inner buffer that contain the value
			case 0:
				file.write((char *)row[i], 4); break;
			case 1:
				file.write((char *)row[i], 8); break;
			case 2:
				file.write((char *)row[i], 1); break;
			case 3:
				file.write((char *)row[i], 32); break;
			case 4:
				file.write((char *)row[i], 64); break;
		}
	}
	file.seekp(3);
	++rowcount;
	file.write((char *)&rowcount, 2);
	file.seekp(0);
}
void vdb::Table::insert_into(std::string &values)
{

	file.seekp(0, std::ios::end);
	// Values example: "12, 43.34, \"String\", \'C\'"
	ltrim(values); // without it mechanism crashes because iterator can't be decremented under the condition.begin()
	rtrim(values);
	for (auto it = values.begin(); it != values.end(); ++it)
	{
		if (*it == '`')
		{
			while (*(++it) != '`')
				if (*it == '\\' && (it + 1) != values.end() && (it + 2) != values.end())
					++it;
		}
		else if (*it == '\"')
		{
			while (*(++it) != '\"')
				if (*it == '\\' && (it + 1) != values.end() && (it + 2) != values.end())
					++it;
		}
		else if (*it == ' ')
		{
			values.erase(it--);
		}
	}

	for (size_t i = 0; i < colcount; ++i)
	{
		size_t k = 0;
		for (; values[k] != ',' && k < values.length(); ++k)
		{
			if (values[k] == '\"')
			{
				while (values[++k] != '\"')
				{
					if (values[k] == '\\' && (k + 1) < values.length() && (k + 2) != values.length())
						++k;
				}
			}
			if (values[k] == '\'')
			{
				if(values[k + 1] == '\\')
					k += 3;
				else
					k += 2;
			}
			
		}
		std::string str_val = values.substr(0, k);
		if (k < values.length())
			values = values.substr(k + 1);

		switch (cols[i].type)
		{
			case 0:
			{
				int val = std::stoi(str_val);
				file.write((char *)&val, 4);
			}; break;
			case 1:
			{
				double val = std::stod(str_val);
				file.write((char *)&val, 8);
			}; break;
			case 2:
			{
				if (str_val[1] == '\\')
					file.write((char *)&str_val[2], 1);
				else 
					file.write((char *)&str_val[1], 1);
			}; break;
			case 3:
			case 4:
			{
				unescape(str_val);
				file.write(str_val.substr(1, str_val.length() - 2).c_str(), cols[i].size);
			}; break;

			default:
				break;
		}

	}
	file.seekp(3);
	++rowcount;
	file.write((char *)&rowcount, 2);
	file.seekp(0);
}
void vdb::Table::insert_into(const char *values)
{
	std::string vals(values);
	insert_into(vals);
}

// Read
vdb::Response vdb::Table::select_all()
{
	vdb::Row *rows = new vdb::Row[rowcount];

	file.seekg(meta_size);
	for (size_t i = 0; i < rowcount; ++i)
	{
		rows[i].resize(colcount);
		for (int j = 0; j < colcount; ++j)
		{
			switch (cols[j].type)
			{
				case 0:
				{
					int buff;
					file.read((char *)&buff, 4);
					rows[i].push_back(buff);
				}; break;
				case 1:
				{
					double buff;
					file.read((char *)&buff, 8);
					rows[i].push_back(buff);
				}; break;
				case 2:
				{
					char buff;
					file.read(&buff, 1);
					rows[i].push_back(buff);
				}; break;
				case 3:
				{
					char buff[32];
					file.read(buff, 32);
					rows[i].push_back(buff);
				}; break;
				case 4:
				{
					char buff[64];
					file.read(buff, 64);
					rows[i].push_back(buff);
				}; break;
			}
		}
	}


	vdb::Response resp(rows, rowcount);

	file.seekg(0);
	delete[] rows;

	return resp;
}
// TODO INSIDE!!!
vdb::Response vdb::Table::select_where(std::string &condition)
{
	// All excess blanks must be truncated
	ltrim(condition); // without it mechanism crashes because iterator can't be decremented under the condition.begin()
	for (auto it = condition.begin(); it != condition.end(); ++it) // TODO: The leading blank will fuck my algorithm
	{
		if (*it == '`')
		{
			while (*(++it) != '`')
				if (*it == '\\' && (it + 1) != condition.end() && (it + 2) != condition.end())
					++it;
		}
		else if (*it == '\"')
		{
			while (*(++it) != '\"')
				if (*it == '\\' && (it + 1) != condition.end() && (it + 2) != condition.end())
					++it;
		}
		else if (*it == ' ')
		{
			condition.erase(it--);
		}
	}
	Node *root = new Node;

	vdb::Response resp = select_all();

	set_tree(condition, root, *this);

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

	return resp;
}
vdb::Response vdb::Table::select_where(const char *condition)
{
	std::string cond(condition);
	return select_where(cond);
}


// Update...


// Delete
void vdb::Table::clear()
{
	if (opened)
	{
		char *buff = new char[meta_size];

		file.read(buff, meta_size);

		file.close();
		file.open(file_name, std::ios::binary | std::ios::out | std::ios::in | std::ios::trunc);

		rowcount = 0;

		buff[3] = 0; // to set rowcount zero
		buff[4] = 0;

		file.write(buff, meta_size);
		file.seekp(0);
		delete[] buff;
	}
}
void vdb::Table::remove_line(size_t line)
{
	if (line < 0 || line >= rowcount || !opened) // line cannot be negative (size_t), maybe in the future this can be useful
		return;

	size_t db_size = meta_size + (rowsize * rowcount);

	char *buffer = new char[db_size];

	file.read(buffer, db_size);

	file.close();
	file.open(file_name, std::ios::binary | std::ios::out | std::ios::in | std::ios::trunc);

	size_t part_1 = meta_size + (rowsize * line);
	size_t part_2 = db_size - part_1 - rowsize;

	file.write(buffer, part_1);
	file.write(buffer + part_1 + rowsize, part_2);

	delete[] buffer;

	file.seekp(3);
	--rowcount;
	file.write((char *)&rowcount, 2);
	file.seekp(0);
}

// Utils
std::string vdb::Table::get_col_name(uint8_t col_index) const
{
	if (opened && (col_index < colcount))
		return std::string(cols[col_index].name);
	else
		throw std::invalid_argument("Argument is more than colcount or table is not opened!");
}
bool vdb::Table::is_open() const
{
	return opened;
}

// Get meta information
uint8_t vdb::Table::get_colcount() const
{
	if (opened)
		return colcount;
	else
		return 0;
}
uint16_t vdb::Table::get_rowcount() const
{
	if (opened)
		return rowcount;
	else
		return 0;
}