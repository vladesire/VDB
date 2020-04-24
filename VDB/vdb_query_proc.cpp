#include "vdb_query_proc.h"

#include "vdb_utils.h"

bool vdb_impl::is_match(vdb::Row &row, const Node *tree)
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

void vdb_impl::destroy_tree(Node *node)
{
	if (node)
	{
		destroy_tree(node->left);
		destroy_tree(node->right);
		delete node;
	}
}

void vdb_impl::rem_brackets(std::string &str)
{
	if (str[0] == '(' && str[str.length() - 1] == ')')
	{
		// If there's ')' braket, but '(' braket (open = false) doesn't accured, no action should be done
		bool open = false, error = false;
		uint8_t nesting_level = 0;
		// skip first and last character
		for (size_t i = 1; i < str.length() - 1; ++i)
		{
			if (str[i] == '(')
			{
				++nesting_level;
			}
			else if (str[i] == ')')
			{
				if (nesting_level == 0)
				{
					error = true;
					break;
				}
				else
				{
					--nesting_level;
				}
			}
		}

		if (!error)
			str = str.substr(1, str.length() - 2);

	}
}

void vdb_impl::set_tree(std::string str, Node *tree, const vdb::Table &table)
{
	uint8_t nesting_level = 0;
	bool operations_left = false;

	// To remove wrapping brackets, for example (a > b), crucial for function to work
	// It should also distingiush something like (a == b) && (c < d) from wrapping brackets
	rem_brackets(str);


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
				vdb::Value val(0);
				tree->value = val;
			}

		}
	}
}
