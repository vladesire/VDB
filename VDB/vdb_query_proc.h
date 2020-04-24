#ifndef VDB_QUERY_PROC_H
#define VDB_QUERY_PROC_H

#include <variant>

#include "vdb_table.h"
#include "vdb_row.h"

namespace vdb_impl
{

struct Node
{
	std::variant<vdb::Value, uint8_t, char> value;
	Node *right;
	Node *left;
	Node()
	{
		right = left = nullptr;
	}
};

bool is_match(vdb::Row &row, const Node *tree);

void destroy_tree(Node *node);

void rem_brackets(std::string &str);

//TODO INSIDE
void set_tree(std::string str, Node *tree, const vdb::Table &table);

}

#endif // !VDB_QUERY_PROC_H
