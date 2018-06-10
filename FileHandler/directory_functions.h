#pragma once

#define WIN32_LEAN_AND_MEAN

#include<windows.h>
#include<vector>
///
//directory reading functions
///
std::string Wchar_to_string(WCHAR* wch);
LPCWSTR string_to_w_char(std::string str);
void read_directory(const std::string& name, std::vector<std::string>& v);
std::string get_current_folder(std::string dir);