#include"directory_functions.h"

std::string Wchar_to_string(WCHAR* wch)
{
	std::string ret = "";

	while (*wch)
		ret += *wch++;

	return ret;
}
LPCWSTR string_to_w_char(std::string str)
{
	WCHAR* ret = new WCHAR[str.length()];

	int i = 0;
	for (; i < str.length(); i++)
		ret[i] = (WCHAR)str[i];
	ret[i] = '\0';

	return ret;
}
void read_directory(const std::string& name, std::vector<std::string>& v)
{
	std::string pattern(name);
	if (pattern.find_last_of("\\") == (pattern.length() - 1))
		pattern.append("*");
	else if (pattern.find_last_of("\\*") != (pattern.length() - 1))
		pattern.append("\\*");
	WIN32_FIND_DATA data;
	HANDLE hFind;
	if ((hFind = FindFirstFile(string_to_w_char(pattern), &data)) != INVALID_HANDLE_VALUE) {
		do {
			v.push_back(Wchar_to_string(data.cFileName));

		} while (FindNextFile(hFind, &data) != 0);
		FindClose(hFind);
	}
}
std::string get_current_folder(std::string dir)
{
	return dir.substr(dir.find_last_of('\\') + 1);
}