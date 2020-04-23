#include "vdb_utils.h"


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

void remove_spaces(std::string &str)  // all non-single spaces, leading and ending spaces are deleted
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
