#ifndef VDB_UTILS_H_
#define VDB_UTILS_H_

#include <string>

inline void ltrim(std::string &s)
{
	s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int ch)
	{
		return !std::isspace(ch);
	}));
}
inline void rtrim(std::string &s)
{
	s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch)
	{
		return !std::isspace(ch);
	}).base(), s.end());
}

void unescape(std::string &str); // \" -> ", \' -> ', etc.
void remove_spaces(std::string &str); // all non-single spaces, leading and ending spaces are deleted
std::string next_token(std::string &source, const char *delim);


#endif // !VDB_UTILS_H_
