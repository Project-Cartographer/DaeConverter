#pragma once
#include<string>

const char DFBT[] = "dfbt";

//stupid windows already has a defintion
//and causes conflicts with tinyxml2
//typedef unsigned int unsigned int;
typedef unsigned __int16 ushort;

char* get_directory_from_file_path(const char* file_path);
char* get_file_from_file_path(char* file_path);

struct dfbt
{
	long long padding;
	unsigned int block_count;
	unsigned int block_size;
};
struct dfbt_list
{
	dfbt* DFBT;
	dfbt_list* next_header;
};
struct tag_data_struct
{
	char* tag_data;
	unsigned int size;
};


