#include<iostream>
#include<conio.h>
#include<fstream>

#include"mode_obj.h"
#include"render_model_import.h"

using std::ifstream;
using std::ofstream;
using std::ios;
using std::getchar;
using std::cout;

void main(int argc,char* args[])
{
	if (argc == 3)
	{
		char temp[256];
		std::string directory;
		GetCurrentDirectoryA(sizeof(temp), temp);
		directory = temp;
		directory += "\\";
		
		//compile dae to render_model
		if (!strcmp(args[1], "-compile"))
		{
			std::string mode_dir = directory + args[2];

			cout << "Compiling file " << mode_dir.c_str() << '\n';

			render_model_import::render_model_import* my_obj = new render_model_import::render_model_import(mode_dir);
			mode* my_render_model = new mode(*my_obj);
			
			string temp = mode_dir + "\\" + get_current_folder(mode_dir) + ".render_model";
			my_render_model->Dump_render_model(temp);
			cout << "Compiled file \n";
		}
		//decompile render_model to obj and dae(BETA)
		else if (!strcmp(args[1], "-decompile"))
		{
			std::string model_loc = directory + args[2];

			ifstream fin;
			fin.open(model_loc.c_str(), ios::binary | ios::in | ios::ate);
			
			if (!fin.is_open())
			{
				cout << "Couldnt find file " << model_loc.c_str() << "\n";
				cout << "Usage : -decompile <render_model file>";
				return;
			}
			unsigned int mode_size = fin.tellg();
			fin.seekg(0x0, ios::beg);

			char* mode_data = new char[mode_size];
			fin.read(mode_data, mode_size);

			fin.close();

			tag_data_struct my_struct;
			my_struct.size = mode_size;
			my_struct.tag_data = mode_data;

			mode* my_mode_obj = new mode(my_struct);

			string output_dir = get_directory_from_file_path(model_loc.c_str());
			output_dir += "\\extract\\";

			CreateDirectoryA(output_dir.c_str(), 0);

			my_mode_obj->Test_Dump(output_dir);
		}
	}
}

