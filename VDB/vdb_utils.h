#ifndef VDB_UTILS_H_
#define VDB_UTILS_H_

#include <string>

inline void unescape(std::string &str) // \\ -> \, \" -> ", \' -> ' 
{
	for (auto it = str.begin(); it != str.end(); ++it)
		if (*it == '\\')
			str.erase(it);
}
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
inline void trim(std::string &s)
{
	ltrim(s);
	rtrim(s);
}
inline std::string trim(const std::string &s)
{
	std::string str{s};
	ltrim(str);
	rtrim(str);
	return str;
}

#endif // !VDB_UTILS_H_
