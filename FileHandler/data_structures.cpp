#include"data_structures.h"

char* get_directory_from_file_path(const char* file_path)
{
	char* dir = new char[strlen(file_path)];
	strcpy(dir, file_path);

	char* temp = dir + strlen(file_path) - 1;

	while (*temp != '\\')
	{
		temp--;
	}
	*temp = '\0';

	return dir;
}
char* get_file_from_file_path(char* file_path)
{
	char* file = new char[strlen(file_path)];
	
	char* temp = file_path;
	char* start = temp;

	while (*temp)
	{
		if (*temp = '\\')
			start = temp + 1;
		temp++;
	}
	strcpy(file, start);
	return file;
}