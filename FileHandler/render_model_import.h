#pragma once
#include"directory_functions.h"
#include<string.h>
#include<vector>

namespace render_model_import
{
	struct premutations
	{
		std::string name;
		std::vector<std::string> model_files;
	};
	struct regions
	{
		std::string name;
		std::vector<premutations> perms_list;
	};

	class render_model_import
	{
	public:
		std::string model_name;
		std::string armature_file;
		std::string markers_file;
		std::vector<regions> region_list;

		render_model_import(std::string model_dir);
	};
}
