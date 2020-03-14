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
	//bullshit test
	//jms::jms jms_obj("C:\\Users\\asus\\Desktop\\general's\\jms\\data\\objects\\characters\\elite\\render\\L1_elite.jms");

	if (argc >=3)
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

			cout << "Compiling DAE file " << mode_dir.c_str() << '\n';

			render_model_import::render_model_import* my_obj = new render_model_import::render_model_import(mode_dir);
			mode* my_render_model = new mode(*my_obj);
			
			string temp = mode_dir + "\\" + get_current_folder(mode_dir) + ".render_model";
			my_render_model->Dump_render_model(temp);

			cout << "Compiled file : " << temp << "\n";

			//delete my_render_model;
			//delete my_obj;
		}
		else if (!strcmp(args[1], "-compileJMS"))
		{
			std::string mode_dir = directory + args[2];

			cout << "Compiling JMS file " << mode_dir.c_str() << '\n';

			render_model_import::jms_model_import *my_obj = new render_model_import::jms_model_import(mode_dir);
			mode* my_render_model = new mode(*my_obj);

			string temp = directory + "\\" + get_current_folder(mode_dir) + ".render_model";
			my_render_model->Dump_render_model(temp);

			cout << "Compiled file : " << temp << "\n";

			//delete my_render_model;
			//delete my_obj;
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

			delete my_mode_obj;
		}
		else if ((!strcmp(args[1], "-replace-node")))
		{
			std::string dest_loc = directory + args[2];
			std::string source_loc = directory + args[3];

			cout << "\n Copying node_data from " << source_loc.c_str() << " to " << dest_loc.c_str();

			ifstream fin;
			fin.open(dest_loc.c_str(), ios::binary | ios::in | ios::ate);
			if(!fin.is_open())
			{
				cout << "Couldnt find file " << dest_loc.c_str() << "\n";
				return;
			}
			tag_data_struct dest_struct;

			dest_struct.size = fin.tellg();
			fin.seekg(0x0, ios::beg);
			dest_struct.tag_data = new char[dest_struct.size];
			fin.read(dest_struct.tag_data, dest_struct.size);
			
			fin.close();

			fin.open(source_loc.c_str(), ios::binary | ios::in | ios::ate);
			if (!fin.is_open())
			{
				cout << "Couldnt find file " << source_loc.c_str() << "\n";
				return;
			}
			tag_data_struct source_struct;

			source_struct.size = fin.tellg();
			fin.seekg(0x0, ios::beg);
			source_struct.tag_data = new char[source_struct.size];
			fin.read(source_struct.tag_data, source_struct.size);

			fin.close();

			mode* dest_mode_obj = new mode(dest_struct);
			mode* source_mode_obj = new mode(source_struct);
			
			replace_node_data(dest_mode_obj, source_mode_obj);

			dest_mode_obj->Dump_render_model(dest_loc);

			delete dest_mode_obj;
			delete source_mode_obj;
		}
		else if ((!strcmp(args[1], "-replace-marker")))
		{
			std::string dest_loc = directory + args[2];
			std::string source_loc = directory + args[3];

			cout << "\n Copying marker_data from " << source_loc.c_str() << " to " << dest_loc.c_str();

			ifstream fin;
			fin.open(dest_loc.c_str(), ios::binary | ios::in | ios::ate);
			if (!fin.is_open())
			{
				cout << "Couldnt find file " << dest_loc.c_str() << "\n";
				return;
			}
			tag_data_struct dest_struct;

			dest_struct.size = fin.tellg();
			fin.seekg(0x0, ios::beg);
			dest_struct.tag_data = new char[dest_struct.size];
			fin.read(dest_struct.tag_data, dest_struct.size);

			fin.close();

			fin.open(source_loc.c_str(), ios::binary | ios::in | ios::ate);
			if (!fin.is_open())
			{
				cout << "Couldnt find file " << source_loc.c_str() << "\n";
				return;
			}
			tag_data_struct source_struct;

			source_struct.size = fin.tellg();
			fin.seekg(0x0, ios::beg);
			source_struct.tag_data = new char[source_struct.size];
			fin.read(source_struct.tag_data, source_struct.size);

			fin.close();

			mode* dest_mode_obj = new mode(dest_struct);
			mode* source_mode_obj = new mode(source_struct);

			replace_marker_stuff(dest_mode_obj, source_mode_obj);

			dest_mode_obj->Dump_render_model(dest_loc);

			delete dest_mode_obj;
			delete source_mode_obj;
		}
	}
}

