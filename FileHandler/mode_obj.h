#pragma once
/*
-Completed the loading part of the render_model 21/2/18,a bit broken
-Completed render_model dumping function at 04/04/18
-Tangent and Normal generation logic taken from https://github.com/Sigmmma/reclaimer/blob/d7433f6e93a71bbbfd3c214cc10e38e5af322c25/reclaimer/model/jms/model.py#L65 at 19/03/20

-Incomplete
	-collada dumping function
	-collada loading function
*/

#include<iostream>
#include<conio.h>
#include<fstream>
#include<string>
#include<vector>
#include<list>

#include"data_structures.h"
#include"render_model_import.h"
#include"jms.h"
#include"JMS_intermediate.h"

//was pissed off due to numerous problems therefore imported everything
#include "assimp/cimport.h"
#include "assimp/scene.h"
#include "assimp/postprocess.h"
#include "assimp/cexport.h"
#include "assimp/ai_assert.h"
#include "assimp/cfileio.h"
#include "assimp/Exporter.hpp"
#include "assimp/importerdesc.h"
#include "assimp/Importer.hpp"
#include "assimp/types.h"

using std::cout;
using std::ifstream;
using std::ofstream;
using std::ios;
using std::string;
using std::to_string;
using std::vector;
using std::list;
using std::reverse;

dfbt* Get_dfbt_from_size(dfbt_list*, unsigned int block_size, char* start_off);
dfbt_list* List_block_headers(char* tag_data, unsigned int size);
string Get_string(char* mem, int length);

struct vector2d
{
	float x;
	float y;
};
struct vector3d
{
	float x;
	float y;
	float z;

	vector3d& operator+(const vector3d other)
	{
		x = x + other.x;
		y = y + other.y;
		z = z + other.z;
		return *this;
	}
	vector3d& operator*(const float scalar)
	{
		x = x*scalar;
		y = y*scalar;
		z = z*scalar;

		return *this;
	}
	void normalize()
	{
		float len = sqrt(x*x + y*y + z*z);
		x = x / len;
		y = y / len;
		z = x / len;
	}
};
struct quaternion
{
	float i;
	float j;
	float k;
	float w;
};
struct node_info
{
	int index;
	float weight;
};
struct material
{
	string shader;
};
struct RAW_vertex
{
	vector3d pos;
	node_info nodes[4];
	vector2d tex_cord;
	vector3d normal;
	vector3d binormal;
	vector3d tangent;
	vector3d anisotropic_binormal;
};
struct strip_index
{
	__int16 vertex_index;
};
struct triangle_strips
{
	vector<strip_index> strip_list;
};
struct node_map
{
	__int8 node_index;
};
struct triangle_face
{
	__int16 v0;
	__int16 v1;
	__int16 v2;
};
struct parts
{
	__int16 material_index;
	__int16 start_index;
	__int16 length;
	vector3d centroid;
	node_info nodes[4];
	vector<triangle_face> face_list;
};
struct permutations
{
	string name;

	__int8 L1;
	__int8 L2;
	__int8 L3;
	__int8 L4;
	__int8 L5;
	__int8 L6;

	//default constructor
	permutations();
};
struct regions
{
	string name;
	vector<permutations> perms;

	//default constructor
	regions();
};
struct nodes
{
	string name;

	__int16 parentNode;
	__int16 firstChildNode;
	__int16 nextSiblingNode;
	__int16 importNodeIndex;
	vector3d defaultTranslation;
	quaternion defaultRotation;
	vector3d inverseForward;
	vector3d inverseLeft;
	vector3d inverseUp;
	vector3d inversePosition;
	float inverseScale;
	float distanceFromParent;
};
struct markers
{
	__int8 regionIndex;
	__int8 permutationIndex;
	__int8 nodeIndex;
	vector3d translation;
	quaternion rotation;
	float scale;
};
struct markers_group
{
	string name;
	vector<markers> markers_list;
};

//section and section_data have been used as equivalent in this case
class section_data
{
public :
	section_data();
	section_data(dfbt*, dfbt_list*,bool);
	section_data(const aiScene*,int material_start_index, const vector<nodes> &nodes_list);
	~section_data();

	vector<parts> parts_list;
	vector<RAW_vertex> vertex_list;
	vector<strip_index> strip_index_list;
	vector<node_map> node_map_list;

	void Generate_Faces(bool);
	void Calculate_vertex_normal_tangent_and_binormal();
	void Remove_redundant_vertices();
	void Remove_redundant_vertices(vector<triangle_face>& face_list, vector<RAW_vertex>& vertex_list, int vertex_start_off = 0, float threshold = 0.001f);
	int _get_node_map_by_node_name(std::string bone_name, const vector<nodes> &nodes_list);
private:
	void Load_mesh_recursive(aiNode* node, const aiScene* my_scene, const vector<nodes> &nodes_list);//utilised during dae loading	
	int access_index;

	int material_start_index;//utilised during dae loading
};
class mode
{
public:
	string name;
	mode(render_model_import::render_model_import&);//load from collada files
	mode(render_model_import::jms_model_import&);//load from jms files
	mode(tag_data_struct&);//constructor doesnt copy,use the existing data in the memory
	~mode();//unallocates the memory for the mode_data

	void Test_Dump(string dir)
	{
		Dump_collada_data(dir + "\\armature.dae");
		
		for (int i = 0; i < region_list.size(); i++)
		{
			std::string reg_dir = dir + "\\" + region_list[i].name;
			CreateDirectoryA(reg_dir.c_str(), 0);

			for (int j = 0; j < region_list[i].perms.size(); j++)
			{
				std::string perm_dir = reg_dir + "\\" + region_list[i].perms[j].name;
				CreateDirectoryA(perm_dir.c_str(), 0);

				Dump_obj_data(section_data_list[region_list[i].perms[j].L1], perm_dir + "\\L1.obj");
				Dump_obj_data(section_data_list[region_list[i].perms[j].L2], perm_dir + "\\L2.obj");
				Dump_obj_data(section_data_list[region_list[i].perms[j].L3], perm_dir + "\\L3.obj");
				Dump_obj_data(section_data_list[region_list[i].perms[j].L4], perm_dir + "\\L4.obj");
				Dump_obj_data(section_data_list[region_list[i].perms[j].L5], perm_dir + "\\L5.obj");
				Dump_obj_data(section_data_list[region_list[i].perms[j].L6], perm_dir + "\\L6.obj");
			}
		}
	}

	void Dump_render_model(string file_loc);

private:
	char* mode_data;
	unsigned int mode_size;

	vector<regions> region_list;
	vector<section_data> section_data_list;
	vector<nodes> nodes_list;
	vector<markers_group> markers_groups_list;
	vector<material> material_list;

	vector3d Calculate_centroid(parts& my_parts,section_data& my_section);
	list<int> Generate_strip_indices(parts& my_parts);

	//Assimp based functions
	void Load_bone(std::string name, const aiScene* my_scene);
	void Link_bones(const aiScene* my_scene);

	//jms node function
	void Load_bones(jms::jms*);
	///marker
	void Load_markers(jms::jms*);
	int _get_marker_group(std::string);

	int find_node_in_node_list(std::string name);//returns the index of node

	///JMS based functions
	//function returns a section index based according to the declarations in material field
	int _get_section_index(const jms::material&);
	//functions returns a material index based as per the subsequent data(LOD,Region,Perm) supplied
	int _get_material_index(const jms::material&);

	void Dump_obj_data(section_data&, string file);	
	void Dump_collada_data(string file);
	void nodes_data_dump(int, aiNode*, aiMesh*);

	//friend functions
	friend void replace_node_data(mode* dest, mode* src);
	friend void replace_marker_stuff(mode* dest, mode* src);
};