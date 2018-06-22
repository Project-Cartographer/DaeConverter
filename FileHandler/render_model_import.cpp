#include "render_model_import.h"
#include<fstream>

namespace render_model_import
{
	render_model_import::render_model_import(std::string model_dir)
	{
		//some stuff on armature and markers
		std::ifstream fin(model_dir + "\\armature.dae");
		if (fin.is_open())
			armature_file = model_dir + "\\armature.dae";
		else
			armature_file = "";
		fin.close();
		fin.open(model_dir + "\\markers.dae");
		if (fin.is_open())
			markers_file = model_dir + "\\markers.dae";
		else markers_file = "";
		fin.close();

		model_name = get_current_folder(model_dir);
		std::vector<std::string> region_names;
		read_directory(model_dir, region_names);

		for (int i = 2; i < region_names.size(); i++)//first two elements arent of our intrest
		{
			std::vector<std::string> perms_names;
			read_directory(model_dir + "\\" + region_names[i], perms_names);

			regions t_region;
			t_region.name = region_names[i];
			for (int j = 2; j < perms_names.size(); j++)
			{
				std::vector<std::string> models;
				read_directory(model_dir + "\\" + region_names[i] + "\\" + perms_names[j], models);

				premutations t_perms;
				t_perms.name = perms_names[j];
				for (int k = 2; k < models.size(); k++)
				{
					t_perms.model_files.push_back(model_dir + "\\" + region_names[i] + "\\" + perms_names[j] + "\\" + models[k]);
				}

				t_region.perms_list.push_back(t_perms);
			}
			region_list.push_back(t_region);
		}
	}
}