#include"mode_obj.h"
#include"TAG_BLOCK_structures.h"

dfbt_list* List_block_headers(char* tag_data, unsigned int size)
{
	//first dfbt starts at 0x40
	dfbt* my_dfbt = (dfbt*)((char*)tag_data + 0x40);

	dfbt_list* ret = new dfbt_list();
	ret->DFBT = my_dfbt;
	ret->next_header = nullptr;

	dfbt_list* current_header_in_action = ret;

	for (int i = 0x50; i < size; i++)
	{
		if (*(tag_data + i) == DFBT[0])
		{
			if (*(tag_data + i + 1) == DFBT[1])
			{
				if (*(tag_data + i + 2) == DFBT[2])
				{
					if (*(tag_data + i + 3) == DFBT[3])
					{
						//well found another dfbt
						dfbt* my_dfbt = (dfbt*)((char*)tag_data + i);

						dfbt_list* temp = new dfbt_list();
						temp->DFBT = my_dfbt;
						temp->next_header = nullptr;

						current_header_in_action->next_header = temp;

						current_header_in_action = temp;
					}
				}
			}
		}
	}
	return ret;
}
dfbt* Get_dfbt_from_size(dfbt_list* list, unsigned int block_size, char* mem_start)
{
	while (list)
	{
		if ((list->DFBT->block_size == block_size) && (((char*)list->DFBT) > mem_start))
			return list->DFBT;

		list = list->next_header;
	}
	return nullptr;
}
string Get_string(char* mem, int length)
{
	string ret = "";

	while (length--)
		ret += *(char*)(mem++);
	
	return ret;
}
section_data::section_data(dfbt* header_ptr, dfbt_list* my_dfbt_list,bool uncompressed)
{
	
	access_index = 0;

	//time to read the count
	char* section_data = (char*)header_ptr + 0x10;
	unsigned int parts_count = *(unsigned int*)(section_data);//off:0x0
	///IDK what is its use
	//unsigned int subparts_count = *(section_data + 0xC);//off:0xc
	unsigned int RAW_vertex_count = *(unsigned int*)(section_data + 0x24);
	unsigned int strip_index_count = *(unsigned int*)(section_data + 0x30);
	unsigned int Node_map_count = *(unsigned int*)(section_data + 0xA4);

	//load the parts shit
	char* mem_start = (char*)header_ptr;
	if (parts_count > 0)
	{
		dfbt* parts_header = Get_dfbt_from_size(my_dfbt_list, 0x48, mem_start);
		char* parts_dat_base = (char*)parts_header + 0x10;

		for (int i = 0; i < parts_count; i++)
		{
			char* parts_dat = parts_dat_base + i * 0x48;
			parts temp;

			temp.material_index = *(__int16*)(parts_dat + 0x4);
			temp.start_index = *(__int16*)(parts_dat + 0x6);
			temp.length = *(__int16*)(parts_dat + 0x8);

			temp.centroid = *(vector3d*)(parts_dat + 0x10);


			for (int i = 0; i < 4; i++)
			{
				temp.nodes[i].index = *(__int8*)(parts_dat + 0x1C + i * 0x4);
				temp.nodes[i].weight = *(float*)(parts_dat + 0x2C + i * 0x4);
			}
			//add to the list
			parts_list.push_back(temp);
		}
		mem_start = (char*)parts_header;
	}
	//load the RAW_vertex_stuff
	if (RAW_vertex_count > 0)
	{
		dfbt* RAW_vertex_header = Get_dfbt_from_size(my_dfbt_list, 0xC4, mem_start);
		char* RAW_vertex_base = (char*)RAW_vertex_header + 0x10;

		for (int i = 0; i < RAW_vertex_count; i++)
		{
			char* RAW_vertex_dat = RAW_vertex_base + i * 0xC4;
			RAW_vertex temp;
			
			temp.pos.x = *(float*)(RAW_vertex_dat + 0x0);
			temp.pos.y = *(float*)(RAW_vertex_dat + 0x4);
			temp.pos.z = *(float*)(RAW_vertex_dat + 0x8);

			for (int i = 0; i < 4; i++)
			{
				temp.nodes[i].index = *(__int8*)(RAW_vertex_dat + 0x2C + i * 0x4);
				temp.nodes[i].weight = *(float*)(RAW_vertex_dat + 0x1C + i * 0x4);
			}

			//*(int*)(RAW_vertex_dat + 0x3C) = 0x1;

			temp.tex_cord.x = *(float*)(RAW_vertex_dat + 0x44);
			temp.tex_cord.y = 1 - *(float*)(RAW_vertex_dat + 0x48);						//<--------some UV crap on its way

			temp.normal.x = *(float*)(RAW_vertex_dat + 0x4C);
			temp.normal.y = *(float*)(RAW_vertex_dat + 0x50);
			temp.normal.z = *(float*)(RAW_vertex_dat + 0x54);

			temp.binormal.x = *(float*)(RAW_vertex_dat + 0x58);
			temp.binormal.y = *(float*)(RAW_vertex_dat + 0x5C);
			temp.binormal.z = *(float*)(RAW_vertex_dat + 0x60);

			temp.tangent.x = *(float*)(RAW_vertex_dat + 0x64);
			temp.tangent.y = *(float*)(RAW_vertex_dat + 0x68);
			temp.tangent.z = *(float*)(RAW_vertex_dat + 0x6C);

			temp.anisotropic_binormal.x = *(float*)(RAW_vertex_dat + 0x70);
			temp.anisotropic_binormal.y = *(float*)(RAW_vertex_dat + 0x74);
			temp.anisotropic_binormal.z = *(float*)(RAW_vertex_dat + 0x78);

			//add to the list
			vertex_list.push_back(temp);
		}
		mem_start = (char*)RAW_vertex_header;
	}
	//Load the strip index data
	if (strip_index_count > 0)
	{
		dfbt* strip_index_header = Get_dfbt_from_size(my_dfbt_list, 0x2, mem_start);
		char* strip_index_base = (char*)strip_index_header + 0x10;

		for (int i = 0; i < strip_index_count; i++)
		{
			char* strip_index_dat = strip_index_base + i * 0x2;
			strip_index temp;

			temp.vertex_index = *(__int16*)(strip_index_dat + 0x0);

			strip_index_list.push_back(temp);
		}
		mem_start = (char*)strip_index_header;
	}
	//Load node_map data
	if (Node_map_count > 0)
	{
		dfbt* node_map_header = Get_dfbt_from_size(my_dfbt_list, 0x1, mem_start);
		char* node_map_base = (char*)node_map_header + 0x10;

		for (int i = 0; i < Node_map_count; i++)
		{
			char* node_map_dat = node_map_base + i * 0x1;
			node_map temp;

			temp.node_index = *(__int8*)(node_map_dat + 0x0);

			node_map_list.push_back(temp);
		}
		mem_start = (char*)node_map_header;
	}
	/*
	i wrote the above parsing codes under the assumption 
	that some of the blocks arent used(usually)
	*/

	Generate_Faces(uncompressed);
}
section_data::~section_data()
{
	
}
void section_data::Generate_Faces(bool uncompressed)
{
	if (!uncompressed)
	{
		//final logic
		int index_start_offset = 0;
		for (int i = 0; i < parts_list.size(); i++)
		{
			int index_buffer_size = parts_list[i].length;//size per part

			if (index_buffer_size >= 3)
			{
				for (int j = 0; j < index_buffer_size - 2; j++)
				{
					triangle_face t_face;
					t_face.v0 = strip_index_list[index_start_offset + j].vertex_index;

					if (j % 2 == 0)
					{
						t_face.v1 = strip_index_list[index_start_offset + j + 1].vertex_index;
						t_face.v2 = strip_index_list[index_start_offset + j + 2].vertex_index;
					}
					else
					{
						t_face.v2 = strip_index_list[index_start_offset + j + 1].vertex_index;
						t_face.v1 = strip_index_list[index_start_offset + j + 2].vertex_index;
					}
					if (t_face.v0 != t_face.v1&&t_face.v1 != t_face.v2&&t_face.v2 != t_face.v0)
						parts_list[i].face_list.push_back(t_face);
				}
			}
			index_start_offset += index_buffer_size;
		}
	}
	else
	{
		for (int k = 0; k < parts_list.size(); k++)
		{
			int indexCount = parts_list[k].length;
			int start_index = parts_list[k].start_index;

			if (indexCount >= 3)
			{
				for (int i = 0; i < (indexCount - 2); i+=3)
				{
					triangle_face temp;

					temp.v0 = strip_index_list[start_index + i].vertex_index;
					temp.v1 = strip_index_list[start_index + i + 1].vertex_index;
					temp.v2 = strip_index_list[start_index + i + 2].vertex_index;

					parts_list[k].face_list.push_back(temp);
				}
			}
		}
	}
}

mode::mode(tag_data_struct& arg0)
{
	mode_data = arg0.tag_data;
	mode_size = arg0.size;

	char* mem_start = (char*)mode_data;
	//load section and its child blocks
	dfbt_list* my_dfbt_list = List_block_headers(mode_data, mode_size);
	dfbt* mode_header = Get_dfbt_from_size(my_dfbt_list, 0xB8, mem_start);

	mem_start = (char*)mode_header;

	///
	//-----------------------------LOADING REGIONS AND PERMUTATIONS---------------------------------------------
	///

	dfbt* regions_header = Get_dfbt_from_size(my_dfbt_list, 0x14, mem_start);
	
	mem_start = (char*)regions_header;

	unsigned int regions_count = regions_header->block_count;
	char* regions_header_mem = (char*)regions_header + 0x10;

	while (regions_count--)
	{
		__int8 region_name_length = *(__int8*)(regions_header_mem + 0x3);
		int permutations_count = *(int*)(regions_header_mem + 0x8);

		//----------------------------AT least ONE Permutation per region is required 
		//TIME TO LOAD THE PERMS
		if (permutations_count)
		{
			regions temp_region;
			char* string_ptr;

			dfbt* permutation_header = Get_dfbt_from_size(my_dfbt_list, 0x10, mem_start);
			char* permutation_header_mem = (char*)permutation_header + 0x10;

			temp_region.name = Get_string((char*)permutation_header - region_name_length, region_name_length);
			string_ptr = permutation_header_mem + permutations_count * 0x10;

			///load up permutation data
			while (permutations_count--)
			{
				__int8 permutation_name_length = *(__int8*)(permutation_header_mem + 0x3);

				permutations temp_permutations;

				temp_permutations.L1 = *(__int16*)(permutation_header_mem + 0x4);
				temp_permutations.L2 = *(__int16*)(permutation_header_mem + 0x4 + 0x2);
				temp_permutations.L3 = *(__int16*)(permutation_header_mem + 0x4 + 0x4);
				temp_permutations.L4 = *(__int16*)(permutation_header_mem + 0x4 + 0x6);
				temp_permutations.L5 = *(__int16*)(permutation_header_mem + 0x4 + 0x8);
				temp_permutations.L6 = *(__int16*)(permutation_header_mem + 0x4 + 0xA);

				temp_permutations.name = Get_string(string_ptr, permutation_name_length);

				temp_region.perms.push_back(temp_permutations);

				string_ptr += permutation_name_length;
				permutation_header_mem += 0x10;
			}

			region_list.push_back(temp_region);

			mem_start = (char*)permutation_header;
		}	
		regions_header_mem += 0x14;

	}
	///End of Regions loading

	///
	//-------------------------------LOADING SECTION_DATA----------------------------------------------------
	///
	
	unsigned int section_count = *(unsigned int*)((char*)mode_header + 0x10 + 0x30);
	dfbt* section_header = Get_dfbt_from_size(my_dfbt_list, 0x68, mem_start);//need to find out the compression flag

	char* section_header_mem = (char*)(section_header)+0x10;
	bool uncompressed = !*(bool*)(section_header_mem + 0x1A);
	//each section_contains only one section_data BLOCK
	while (section_count--)
	{
		//time to look into the section_data
		dfbt* section_data_header = Get_dfbt_from_size(my_dfbt_list, 0xB4, mem_start);
		//Load the section_data and its child blocks
		section_data_list.emplace_back(section_data_header, my_dfbt_list,uncompressed);
		//update mem_start
		mem_start = (char*)section_data_header;
	}	
	///---------------------------------END OF SECTION LOADING

	///
	//----------------------------------NODES LOADING--------------------------------
	///
	unsigned int nodes_count = *(unsigned int*)((char*)mode_header + 0x10 + 0x60);

	if (nodes_count)
	{
		dfbt* nodes_header = Get_dfbt_from_size(my_dfbt_list, 0x60, mem_start);
		char* nodes_header_mem = (char*)nodes_header + 0x10;

		char* string_ptr = nodes_header_mem + 0x60 * nodes_count;

		while (nodes_count--)
		{
			__int8 nodes_name_length = *(__int8*)(nodes_header_mem + 0x3);

			nodes temp_nodes;

			temp_nodes.name = Get_string(string_ptr, nodes_name_length);

			temp_nodes.parentNode = *(__int16*)(nodes_header_mem + 0x4);
			temp_nodes.firstChildNode = *(__int16*)(nodes_header_mem + 0x6);
			temp_nodes.nextSiblingNode = *(__int16*)(nodes_header_mem + 0x8);
			temp_nodes.importNodeIndex = *(__int16*)(nodes_header_mem + 0xA);
			temp_nodes.defaultTranslation = *(vector3d*)(nodes_header_mem + 0xC);
			temp_nodes.defaultRotation = *(quaternion*)(nodes_header_mem + 0x18);
			temp_nodes.inverseForward = *(vector3d*)(nodes_header_mem + 0x28);
			temp_nodes.inverseLeft = *(vector3d*)(nodes_header_mem + 0x34);
			temp_nodes.inverseUp = *(vector3d*)(nodes_header_mem + 0x40);
			temp_nodes.inversePosition = *(vector3d*)(nodes_header_mem + 0x4C);
			temp_nodes.inverseScale = *(float*)(nodes_header_mem + 0x58);
			temp_nodes.distanceFromParent = *(float*)(nodes_header_mem + 0x5C);

			nodes_list.push_back(temp_nodes);

			string_ptr += nodes_name_length;
			nodes_header_mem += 0x60;
		}

		mem_start = (char*)nodes_header;
	}
	///---------------------END OF NODES LOADING

	///
	//-----------------------------------MARKERS LOADING----------------------------------------
	///
	unsigned int marker_group_count = *(unsigned int*)((char*)mode_header + 0x10 + 0x78);

	if (marker_group_count)
	{
		dfbt* marker_group_header = Get_dfbt_from_size(my_dfbt_list, 0x10, mem_start);
		char* marker_group_header_mem = (char*)marker_group_header + 0x10;

		mem_start = (char*)marker_group_header;

		while (marker_group_count--)
		{
			__int8 marker_group_name_length = *(__int8*)(marker_group_header_mem + 0x3);
			unsigned int markers_count = *(unsigned int*)(marker_group_header_mem + 0x4);

			//------At leaast one marker is required per marker_group
			if (markers_count)
			{
				markers_group temp_markers_group;

				dfbt* markers_header = Get_dfbt_from_size(my_dfbt_list, 0x24, mem_start);
				temp_markers_group.name = Get_string((char*)markers_header - marker_group_name_length, marker_group_name_length);

				char* marker_mem = (char*)markers_header + 0x10;

				while (markers_count--)
				{
					markers temp_markers;

					temp_markers.regionIndex = *(__int8*)(marker_mem);
					temp_markers.permutationIndex = *(__int8*)(marker_mem + 0x1);
					temp_markers.nodeIndex = *(__int8*)(marker_mem + 0x2);
					temp_markers.translation = *(vector3d*)(marker_mem + 0x4);
					temp_markers.rotation = *(quaternion*)(marker_mem + 0x10);
					temp_markers.scale = *(float*)(marker_mem + 0x20);

					temp_markers_group.markers_list.push_back(temp_markers);
				}

				markers_groups_list.push_back(temp_markers_group);
				marker_group_header_mem += 0x10;
				mem_start = (char*)markers_header;
			}
		}
		
	}
	///------------------END of MArkers loading

	///
	//----------------------LOADING MATERIALS
	///
	unsigned int material_count = *(unsigned int*)((char*)mode_header + 0x10 + 0x84);

	if (material_count)
	{
		dfbt* material_header = Get_dfbt_from_size(my_dfbt_list, 0x34, mem_start);
		char* material_header_mem = (char*)material_header + 0x10;

		char* string_ptr = material_header_mem + material_count * 0x34;

		//we are basically concerned with the new materials
		while (material_count--)
		{
			material temp_material;

			unsigned int tag_loc_string_length = *(unsigned int*)(material_header_mem + 0x18);
			temp_material.shader = Get_string(string_ptr, tag_loc_string_length);

			material_list.push_back(temp_material);

			material_header_mem += 0x34;
			string_ptr += tag_loc_string_length + 1;
		}
		mem_start = (char*)material_header;
	}

	///i am too lazy to write a dfbt_list deletion logic :P
}
mode::~mode()
{
	delete[] mode_data;
}
void mode::Dump_obj_data(section_data& section_dat, string file)
{
	ofstream fout;
	fout.open(file, ios::out);

	//write vertices pos
	for (int i = 0; i < section_dat.vertex_list.size(); i++)
	{
		float x = section_dat.vertex_list[i].pos.x * 100;
		float y = section_dat.vertex_list[i].pos.y * 100;
		float z = section_dat.vertex_list[i].pos.z * 100;
		fout << "v  " << to_string(x) << ' ' << to_string(y) << ' ' << to_string(z) << '\n';
	}
	fout << "\n";//leave a line
	//write texture coordinates
	for (int i = 0; i < section_dat.vertex_list.size(); i++)
	{
		float x = section_dat.vertex_list[i].tex_cord.x;
		float y = section_dat.vertex_list[i].tex_cord.y;
		float z = 0.0f;
		fout << "vt  " << to_string(x) << ' ' << to_string(y) << ' ' << to_string(z) << '\n';
	}
	fout << "\n";//leave a line
	//write vertex normals
	for (int i = 0; i < section_dat.vertex_list.size(); i++)
	{
		float x = section_dat.vertex_list[i].normal.x;
		float y = section_dat.vertex_list[i].normal.y;
		float z = section_dat.vertex_list[i].normal.z;
		fout << "vn  " << to_string(x) << ' ' << to_string(y) << ' ' << to_string(z) << '\n';
	}
	fout << "\n";//leave a line
	
	//time to write the faces
	for (int k = 0; k < section_dat.parts_list.size(); k++)
	{
		parts temp_part = section_dat.parts_list[k];

		fout << "g group_" << to_string(k + 1) << '\n';

		for (int i = 0; i < temp_part.face_list.size(); i++)
		{
			triangle_face temp = temp_part.face_list[i];

			string tempy = to_string(temp.v0 + 1);
			fout << "f " << tempy << '/' << tempy << '/' << tempy << '\t';
			tempy = to_string(temp.v1 + 1);
			fout << tempy << '/' << tempy << '/' << tempy << '\t';
			tempy = to_string(temp.v2 + 1);
			fout << tempy << '/' << tempy << '/' << tempy << '\n';
		}
		cout << '\n';
	}
	//write the triangle strip data
	for (int k = 0; k < section_dat.parts_list.size(); k++)
	{
		int start_index = section_dat.parts_list[k].start_index;
		int length = section_dat.parts_list[k].length;

		fout << '#';
		for (int i = 0; i < length; i++)
			fout << to_string(section_dat.strip_index_list[start_index + i].vertex_index + 1) << '\t';
		fout << '\n';
	}

	fout.close();
}
//Dumps the render model into a collada file format
void mode::Dump_collada_data(string file)
{
	Assimp::Exporter exporter;
	const aiExportFormatDesc* format = exporter.GetExportFormatDescription(0);

	aiScene *test_scene=new aiScene();

	test_scene->mRootNode = new aiNode();
	test_scene->mRootNode->mName = "root";
	aiIdentityMatrix4(&test_scene->mRootNode->mTransformation);

	if (nodes_list.size())
		nodes_data_dump(0, test_scene->mRootNode);

	aiReturn ret = exporter.Export(test_scene, format->id, file, test_scene->mFlags);
}
//Dumps the render_model node data into the assimp stuff
void mode::nodes_data_dump(int current_node_index, aiNode* parent_node)
{
	aiNode** temp = new aiNode*[parent_node->mNumChildren + 1];

	for (int i = 0; i < parent_node->mNumChildren; i++)
	{
		temp[i] = parent_node->mChildren[i];
	}

	aiNode* t_node= new aiNode();
	t_node->mName = nodes_list[current_node_index].name;
	t_node->mParent = parent_node;
	
	aiMatrix4x4 inv_trans;
	inv_trans.a1 = nodes_list[current_node_index].inverseForward.y;
	inv_trans.a2 = nodes_list[current_node_index].inverseLeft.y;
	inv_trans.a3 = nodes_list[current_node_index].inverseUp.y;
	inv_trans.a4 = nodes_list[current_node_index].inversePosition.y * 100;

	inv_trans.b1 = nodes_list[current_node_index].inverseForward.z;
	inv_trans.b2 = nodes_list[current_node_index].inverseLeft.z;
	inv_trans.b3 = nodes_list[current_node_index].inverseUp.z;
	inv_trans.b4 = nodes_list[current_node_index].inversePosition.z * 100;

	inv_trans.c1 = nodes_list[current_node_index].inverseLeft.x;
	inv_trans.c2 = nodes_list[current_node_index].inverseUp.x;
	inv_trans.c3 = nodes_list[current_node_index].inversePosition.x;
	inv_trans.c4 = nodes_list[current_node_index].inverseScale * 100;

	inv_trans.d1 = 0;
	inv_trans.d2 = 0;
	inv_trans.d3 = 0;
	inv_trans.d4 = 1;

	inv_trans.Inverse();

	if (nodes_list[current_node_index].parentNode != -1)
	{
		aiMatrix4x4 p_inv_trans;

		p_inv_trans.a1 = nodes_list[nodes_list[current_node_index].parentNode].inverseForward.y;
		p_inv_trans.a2 = nodes_list[nodes_list[current_node_index].parentNode].inverseLeft.y;
		p_inv_trans.a3 = nodes_list[nodes_list[current_node_index].parentNode].inverseUp.y;
		p_inv_trans.a4 = nodes_list[nodes_list[current_node_index].parentNode].inversePosition.y * 100;

		p_inv_trans.b1 = nodes_list[nodes_list[current_node_index].parentNode].inverseForward.z;
		p_inv_trans.b2 = nodes_list[nodes_list[current_node_index].parentNode].inverseLeft.z;
		p_inv_trans.b3 = nodes_list[nodes_list[current_node_index].parentNode].inverseUp.z;
		p_inv_trans.b4 = nodes_list[nodes_list[current_node_index].parentNode].inversePosition.z * 100;

		p_inv_trans.c1 = nodes_list[nodes_list[current_node_index].parentNode].inverseLeft.x;
		p_inv_trans.c2 = nodes_list[nodes_list[current_node_index].parentNode].inverseUp.x;
		p_inv_trans.c3 = nodes_list[nodes_list[current_node_index].parentNode].inversePosition.x;
		p_inv_trans.c4 = nodes_list[nodes_list[current_node_index].parentNode].inverseScale * 100;

		p_inv_trans.d1 = 0;
		p_inv_trans.d2 = 0;
		p_inv_trans.d3 = 0;
		p_inv_trans.d4 = 1;

		aiMultiplyMatrix4(&inv_trans, &p_inv_trans);
	}
	t_node->mTransformation = inv_trans;//actually its the transformation

	temp[parent_node->mNumChildren] = t_node;

	delete [] parent_node->mChildren;
	parent_node->mChildren = temp;	
	parent_node->mNumChildren += 1;
	
	if (nodes_list[current_node_index].nextSiblingNode != -1)
		nodes_data_dump(nodes_list[current_node_index].nextSiblingNode, parent_node);
	if (nodes_list[current_node_index].firstChildNode != -1)
		nodes_data_dump(nodes_list[current_node_index].firstChildNode, t_node);
		
}
//Dumps the loaded model data into a render_model file
void mode::Dump_render_model(string file_loc)
{
	ofstream fout(file_loc.c_str(), ios::binary | ios::out);

	if (fout.is_open())
	{
		//write the junk of 0x40 bytes
		int length = sizeof(_mode_base) / sizeof(_mode_base[0]);
		fout.write((char*)_mode_base, length);

		//creating a dfbt header
		dfbt _dfbt;
		_dfbt.padding = 0x74626664;

		//fixing dfbt header for base
		_dfbt.block_count = 0x1;
		_dfbt.block_size = 0xB8;

		fout.write((char*)&_dfbt, sizeof(dfbt));//the header

		//the tag base
		char tag_base[0xB8] = { 0 };
		//modifing the parameters
		*(int*)&tag_base[0x24] = region_list.size();
		*(int*)&tag_base[0x30] = section_data_list.size();
		*(int*)&tag_base[0x60] = nodes_list.size();
		*(int*)&tag_base[0x78] = markers_groups_list.size();
		*(int*)&tag_base[0x84] = material_list.size();

		fout.write(tag_base, 0xB8);			

		if (region_list.size())
		{
			//creating regions header
			_dfbt.block_count = region_list.size();
			_dfbt.block_size = 0x14;

			fout.write((char*)&_dfbt, sizeof(dfbt));
		}

		//writing regions block
		for (int i = 0; i < region_list.size(); i++)
		{
			char regions[0x14] = { 0 };
			regions[0x3] = region_list[i].name.length();
			*(int*)&regions[0x8] = region_list[i].perms.size();
			fout.write(regions, 0x14);
		}
		//writing regions name and permutations
		for (int i = 0; i < region_list.size(); i++)
		{
			fout.write(region_list[i].name.c_str(), region_list[i].name.length());
			//write the perms header
			if (region_list[i].perms.size())
			{
				_dfbt.block_count = region_list[i].perms.size();
				_dfbt.block_size = 0x10;

				fout.write((char*)&_dfbt, 0x10);
			}

			//write the perms block
			for (int j = 0; j < region_list[i].perms.size(); j++)
			{
				char perms[0x10] = { 0 };
				perms[0x3] = region_list[i].perms[j].name.length();
				*(__int16*)&perms[0x4] = region_list[i].perms[j].L1;
				*(__int16*)&perms[0x6] = region_list[i].perms[j].L2;
				*(__int16*)&perms[0x8] = region_list[i].perms[j].L3;
				*(__int16*)&perms[0xA] = region_list[i].perms[j].L4;
				*(__int16*)&perms[0xC] = region_list[i].perms[j].L5;
				*(__int16*)&perms[0xE] = region_list[i].perms[j].L6;

				fout.write(perms, 0x10);
			}
			//write the perms names
			for (int j = 0; j < region_list[i].perms.size(); j++)
				fout.write(region_list[i].perms[j].name.c_str(), region_list[i].perms[j].name.length());
		}
		//---------------------------------at least there should be one section------------------------
		//creating the section header
		_dfbt.block_count = section_data_list.size();
		_dfbt.block_size = 0x68;

		fout.write((char*)&_dfbt, sizeof(dfbt));

		//writing the section block
		for (int i = 0; i < section_data_list.size(); i++)
		{
			char section[0x68] = { 0 };
			*(int*)&section[0x34] = 1;
			fout.write((char*)section, 0x68);
		}
		//writing the section data,each section contains only one section DATA
		for (int i = 0; i < section_data_list.size(); i++)
		{
			fout.write((char*)FINS, 0x10);//starting of section child elements
			
			_dfbt.block_count = 0x1;
			_dfbt.block_size = 0xB4;
			//section data header
			fout.write((char*)&_dfbt, sizeof(dfbt));
			//section data
			char section_data[0xB4] = { 0 };
			*(int*)&section_data[0x0] = section_data_list[i].parts_list.size();
			*(int*)&section_data[0x24] = section_data_list[i].vertex_list.size();

			//calculating stip indexes length
			__int16 strip_indexes_length=0;
			for (int j = 0; j < section_data_list[i].parts_list.size(); j++)
				strip_indexes_length += section_data_list[i].parts_list[j].face_list.size() * 3;

			*(int*)&section_data[0x30] = strip_indexes_length;
			*(int*)&section_data[0xA4] = section_data_list[i].node_map_list.size();
			fout.write(section_data, 0xB4);

			fout.write((char*)TCES, 0x10);//starting of section data child BLOCKs section(global_geometry_section_struct_block)

			//---------------------------well i guess u cannot render anything without parts
			//parts header
			_dfbt.block_count = section_data_list[i].parts_list.size();
			_dfbt.block_size = 0x48;

			fout.write((char*)&_dfbt, sizeof(dfbt));

			//Strip indexes list
			list<int> strip_index_list;

			//write the parts block
			for (int j = 0; j < section_data_list[i].parts_list.size(); j++)
			{
				char parts[0x48] = { 0 };
				list<int> temp_list = Generate_strip_indices(section_data_list[i].parts_list[j]);

				*(__int16*)&parts[0] = 2;
				*(__int16*)&parts[2] = 0x1 | 0x2 | 0x8;
				*(__int16*)&parts[4] = section_data_list[i].parts_list[j].material_index;
				*(__int16*)&parts[6] = strip_index_list.size();
				*(__int16*)&parts[8] = temp_list.size();
				*(vector3d*)&parts[0x10] = Calculate_centroid(section_data_list[i].parts_list[j], section_data_list[i]);

				strip_index_list.insert(strip_index_list.cend(), temp_list.cbegin(), temp_list.cend());

				fout.write(parts, 0x48);
			}
			//---------------------------what is a model file without any vertex
			//RAW vertices header
			_dfbt.block_count = section_data_list[i].vertex_list.size();
			_dfbt.block_size = 0xC4;

			fout.write((char*)&_dfbt, sizeof(dfbt));

			//write the RAW vertex block
			for (int j = 0; j < section_data_list[i].vertex_list.size(); j++)
			{
				char vertex[0xC4] = { 0 };

				RAW_vertex &temp = section_data_list[i].vertex_list[j];

				*(vector3d*)(vertex) = temp.pos;

				for (int i = 0; i < 4; i++)
				{
					*(__int8*)(vertex + 0x2C + i * 0x4) = (__int8)temp.nodes[i].index;
					*(float*)(vertex + 0x1C + i * 0x4) = temp.nodes[i].weight;			
					
				}

				*(int*)(vertex + 0x3C) = 0x1;

				*(float*)(vertex + 0x44) = temp.tex_cord.x;
				*(float*)(vertex + 0x48) = 1 - temp.tex_cord.y;			//<--------some UV crap on its way
						
				*(vector3d*)(vertex + 0x4C) = temp.normal;
				*(vector3d*)(vertex + 0x58) = temp.binormal;
				*(vector3d*)(vertex + 0x64) = temp.tangent;
				*(vector3d*)(vertex + 0x70) = temp.anisotropic_binormal;


				fout.write(vertex, 0xC4);
			}
			if (strip_index_list.size())
			{
				//strip index header
				_dfbt.block_count = strip_index_list.size();
				_dfbt.block_size = 0x2;

				fout.write((char*)&_dfbt, sizeof(dfbt));
			}

			//write the strip index block
			while(strip_index_list.size())
			{
				char strip_index[2] = { 0 };
				*(__int16*)&strip_index[0] = (__int16)*strip_index_list.cbegin();
				strip_index_list.pop_front();
				fout.write(strip_index, 0x2);
			}

			fout.write((char*)TADP, 0x10);//end of section data child BLOCKs section(global_geometry_section_struct_block)

			if (section_data_list[i].node_map_list.size())
			{
				//node map header
				_dfbt.block_count = section_data_list[i].node_map_list.size();
				_dfbt.block_size = 0x1;

				fout.write((char*)&_dfbt, sizeof(dfbt));
			}

			//write the node map header block
			for (int j = 0; j < section_data_list[i].node_map_list.size(); j++)
				fout.write(&section_data_list[i].node_map_list[j].node_index, 0x1);	

			fout.write((char*)KOLB, 0x10);//end of section child elements
		}
		
		if (nodes_list.size())
		{
			//nodes header
			_dfbt.block_count = nodes_list.size();
			_dfbt.block_size = 0x60;

			fout.write((char*)&_dfbt, sizeof(dfbt));
		}

		//nodes writing stuff
		for (int i = 0; i < nodes_list.size(); i++)
		{
			char nodes_header_mem[0x60] = { 0 };

			*(__int8*)(nodes_header_mem + 0x3) = nodes_list[i].name.length();
			*(__int16*)(nodes_header_mem + 0x4) = nodes_list[i].parentNode;
			*(__int16*)(nodes_header_mem + 0x6) = nodes_list[i].firstChildNode;
			*(__int16*)(nodes_header_mem + 0x8) = nodes_list[i].nextSiblingNode;
			*(__int16*)(nodes_header_mem + 0xA) = nodes_list[i].importNodeIndex;
			*(vector3d*)(nodes_header_mem + 0xC) = nodes_list[i].defaultTranslation;
			*(quaternion*)(nodes_header_mem + 0x18) = nodes_list[i].defaultRotation;
			*(vector3d*)(nodes_header_mem + 0x28) = nodes_list[i].inverseForward;
			*(vector3d*)(nodes_header_mem + 0x34) = nodes_list[i].inverseLeft;
			*(vector3d*)(nodes_header_mem + 0x40) = nodes_list[i].inverseUp;
			*(vector3d*)(nodes_header_mem + 0x4C) = nodes_list[i].inversePosition;
			*(float*)(nodes_header_mem + 0x58) = nodes_list[i].inverseScale;
			*(float*)(nodes_header_mem + 0x5C) = nodes_list[i].distanceFromParent;

			fout.write(nodes_header_mem, 0x60);
		}
		//for writing the names of nodes
		for (int i = 0; i < nodes_list.size(); i++)
		{
			fout.write(nodes_list[i].name.c_str(), nodes_list[i].name.length());
		}
		if (markers_groups_list.size())
		{
			//marker group header
			_dfbt.block_count = markers_groups_list.size();
			_dfbt.block_size = 0x10;

			fout.write((char*)&_dfbt, sizeof(dfbt));
		}
		//marker group writing stuff
		for (int i = 0; i < markers_groups_list.size(); i++)
		{
			char mark_g_dat[0x10] = { 0 };

			*(__int8*)(mark_g_dat + 0x3) = markers_groups_list[i].name.length();
			*(unsigned int*)(mark_g_dat + 0x4) = markers_groups_list[i].markers_list.size();

			fout.write(mark_g_dat, 0x10);
		}
		//for writing marker stuff
		for (int i = 0; i < markers_groups_list.size(); i++)
		{
			//marker group name
			fout.write(markers_groups_list[i].name.c_str(), markers_groups_list[i].name.length());

			if (markers_groups_list[i].markers_list.size())
			{
				//marker header
				_dfbt.block_count = markers_groups_list[i].markers_list.size();
				_dfbt.block_size = 0x24;

				fout.write((char*)&_dfbt, sizeof(dfbt));
			}
			//marker data
			for (int j = 0; j < markers_groups_list[i].markers_list.size(); j++)
			{
				char marker_dat[0x24] = { 0 };

				*(__int8*)(marker_dat) = markers_groups_list[i].markers_list[j].regionIndex;
				*(__int8*)(marker_dat + 0x1) = markers_groups_list[i].markers_list[j].permutationIndex;
				*(__int8*)(marker_dat + 0x2) = markers_groups_list[i].markers_list[j].nodeIndex;
				*(vector3d*)(marker_dat + 0x4) = markers_groups_list[i].markers_list[j].translation;
				*(quaternion*)(marker_dat + 0x10) = markers_groups_list[i].markers_list[j].rotation;
				*(float*)(marker_dat + 0x20) = markers_groups_list[i].markers_list[j].scale;

				fout.write(marker_dat, 0x24);
			}
		}
		if (material_list.size())
		{
			//materials header
			_dfbt.block_count = material_list.size();
			_dfbt.block_size = 0x34;

			fout.write((char*)&_dfbt, sizeof(dfbt));
		}

		//materials data
		for (int i = 0; i < material_list.size(); i++)
		{
			char mat_dat[] = { 0x64,0x61,0x68,0x73,0x40,0x11,0x40,0x08,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0x64,0x61,0x68,0x73,0xD0,0x18,0x40,0x08,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0x44,0x4D,0x95,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00 };

			*(unsigned int*)(mat_dat + 0x18) = material_list[i].shader.length();

			fout.write(mat_dat, 0x34);
		}
		for (int i = 0; i < material_list.size(); i++)
		{
			fout.write(material_list[i].shader.c_str(), material_list[i].shader.length() + 1);
		}		
	}

	fout.close();
}
//Calculates the centroid of the specified part
vector3d mode::Calculate_centroid(parts& my_parts,section_data& my_section)
{
	vector3d ret;
	ret.x = ret.y = ret.z = 0;

	for (int i = 0; i < my_parts.face_list.size(); i++)
	{
		RAW_vertex v0 = my_section.vertex_list[my_parts.face_list[i].v0];
		RAW_vertex v1 = my_section.vertex_list[my_parts.face_list[i].v1];
		RAW_vertex v2 = my_section.vertex_list[my_parts.face_list[i].v2];

		ret.x += (v0.pos.x + v1.pos.x + v2.pos.x) / 3;
		ret.y += (v0.pos.y + v1.pos.y + v2.pos.y) / 3;
		ret.z += (v0.pos.z + v1.pos.z + v2.pos.z) / 3;
	}
	ret.x /= my_parts.face_list.size();
	ret.y /= my_parts.face_list.size();
	ret.z /= my_parts.face_list.size();

	return ret;
}
//Generates a strip index list from parts
list<int> mode::Generate_strip_indices(parts& my_parts)
{
	list<int> ret;

	for (int i = 0; i < my_parts.face_list.size(); i++)
	{
		ret.push_back(my_parts.face_list[i].v0);
		ret.push_back(my_parts.face_list[i].v1);
		ret.push_back(my_parts.face_list[i].v2);
	}

	return ret;
}
//normalises the node weigths in the parts
void __vertex_normalise_weigths(RAW_vertex* p)
{
	float magnitude = sqrt(pow(p->nodes[0].weight, 2) + pow(p->nodes[1].weight, 2) + pow(p->nodes[2].weight, 2) + pow(p->nodes[3].weight, 2));
	p->nodes[0].weight /= magnitude;
	p->nodes[1].weight /= magnitude;
	p->nodes[2].weight /= magnitude;
	p->nodes[3].weight /= magnitude;
}

#pragma region model_import
section_data::section_data(const aiScene* my_scene, int material_start_index,const vector<nodes> &nodes_list)
{
	this->material_start_index = material_start_index;

	aiNode* root_node = my_scene->mRootNode;

	for (int i = 0; i < root_node->mNumChildren; i++)
		Load_mesh_recursive(root_node->mChildren[i], my_scene,nodes_list);

	//normalize the node weights
	for (int i = 0; i < vertex_list.size(); i++)
		__vertex_normalise_weigths(&vertex_list[i]);
}
void section_data::Load_mesh_recursive(aiNode* node, const aiScene* my_scene, const vector<nodes> &nodes_list)
{
	int vertex_start_index = vertex_list.size();
	int _per_mesh_nodemap_start_index = node_map_list.size();

	for (int i = 0; i < node->mNumMeshes; i++)
	{
		int mesh_index = node->mMeshes[i];
		aiMesh* t_mesh = my_scene->mMeshes[mesh_index];

		for (int j = 0; j < t_mesh->mNumVertices; j++)
		{
			RAW_vertex t_RAWvertex = { 0 };

			aiVector3D t_vertex_pos = t_mesh->mVertices[j];

			aiNode* t_parent = node;
			while (t_parent!=my_scene->mRootNode)
			{
				const aiMatrix4x4 t_mat = t_parent->mTransformation;
				
				aiTransformVecByMatrix4(&t_vertex_pos, &t_mat);
				t_parent = t_parent->mParent;
			}

			t_RAWvertex.pos.x = t_vertex_pos.x;
			t_RAWvertex.pos.y = t_vertex_pos.y;
			t_RAWvertex.pos.z = t_vertex_pos.z;

			if (t_mesh->HasTangentsAndBitangents())
			{
				aiVector3D t_vertex_binorm = t_mesh->mBitangents[j];
				aiVector3D t_vertex_tan = t_mesh->mTangents[j];

				t_RAWvertex.binormal.x = t_vertex_binorm.x;
				t_RAWvertex.binormal.y = t_vertex_binorm.y;
				t_RAWvertex.binormal.z = t_vertex_binorm.z;
				t_RAWvertex.tangent.x = t_vertex_tan.x;
				t_RAWvertex.tangent.y = t_vertex_tan.y;
				t_RAWvertex.tangent.z = t_vertex_tan.z;
			}
			if (t_mesh->HasNormals())
			{
				aiVector3D t_vertex_norm = t_mesh->mNormals[j];

				t_RAWvertex.normal.x = t_vertex_norm.x;
				t_RAWvertex.normal.y = t_vertex_norm.y;
				t_RAWvertex.normal.z = t_vertex_norm.z;
			}
			if (t_mesh->HasTextureCoords(0))
			{
				aiVector3D t_vertex_uv = t_mesh->mTextureCoords[0][j];

				t_RAWvertex.tex_cord.x = t_vertex_uv.x;
				t_RAWvertex.tex_cord.y = t_vertex_uv.y;//halo math
			}

			vertex_list.push_back(t_RAWvertex);
		}
		//now add the node weights
		//render_model supports upto 4 nodes only
		for (int j = 0; j < t_mesh->mNumBones; j++)
		{
			aiBone* t_bone = t_mesh->mBones[j];
			aiVertexWeight* t_weight = t_bone->mWeights;

			for (int k = 0; k <  t_bone->mNumWeights; k++)
			{
				if (t_weight[k].mWeight > vertex_list[vertex_start_index + t_weight[k].mVertexId].nodes[0].weight)
				{
					vertex_list[t_weight[k].mVertexId].nodes[0].index = _per_mesh_nodemap_start_index + j;
					vertex_list[t_weight[k].mVertexId].nodes[0].weight = t_weight[k].mWeight;
				}
				else if (t_weight[k].mWeight>vertex_list[vertex_start_index + t_weight[k].mVertexId].nodes[1].weight)
				{
					vertex_list[t_weight[k].mVertexId].nodes[1].index = _per_mesh_nodemap_start_index + j;
					vertex_list[t_weight[k].mVertexId].nodes[1].weight = t_weight[k].mWeight;
				}
				else if (t_weight[k].mWeight > vertex_list[vertex_start_index + t_weight[k].mVertexId].nodes[2].weight)
				{
					vertex_list[t_weight[k].mVertexId].nodes[2].index = _per_mesh_nodemap_start_index + j;
					vertex_list[t_weight[k].mVertexId].nodes[2].weight = t_weight[k].mWeight;
				}
				else if (t_weight[k].mWeight > vertex_list[vertex_start_index + t_weight[k].mVertexId].nodes[3].weight)
				{
					vertex_list[t_weight[k].mVertexId].nodes[3].index = _per_mesh_nodemap_start_index + j;
					vertex_list[t_weight[k].mVertexId].nodes[3].weight = t_weight[k].mWeight;
				}
			}
		}

		parts t_part = { 0 };
		//there is a mesh per material in Assimp
		t_part.material_index = material_start_index + (short)t_mesh->mMaterialIndex;

		aiFace* t_face = t_mesh->mFaces;
		for (int j = 0; j < t_mesh->mNumFaces; j++)
		{
			//supposed to handle only three faces comprising of three vertices
			unsigned int* t_indices = t_face[j].mIndices;
			triangle_face t_face = { 0 };
			t_face.v0 = vertex_start_index + t_indices[0];
			t_face.v1 = vertex_start_index + t_indices[1];
			t_face.v2 = vertex_start_index + t_indices[2];

			t_part.face_list.push_back(t_face);
		}
		parts_list.push_back(t_part);

		//adding a node map
		//node map(bones or armature) should be same across LODs and permuatations
		//just a add a crazy node map for a while
		for (int j = 0; j < t_mesh->mNumBones; j++)
		{
			node_map t_Nmap;
			for (int k = 0; k < nodes_list.size(); k++)
			{
				if (strcmp(nodes_list[k].name.c_str(), t_mesh->mBones[j]->mName.C_Str()) == 0)
				{
					t_Nmap.node_index = k;
					break;
				}
			}
			node_map_list.push_back(t_Nmap);
		}

	}

	for (int i = 0; i < node->mNumChildren; i++)
		Load_mesh_recursive(node->mChildren[i], my_scene,nodes_list);

}

mode::mode(render_model_import::render_model_import& my_import)
{
	name = my_import.model_name;
	int section_index = 0;
	int material_start_index = 0;
	for (int i = 0; i < my_import.region_list.size(); i++)
	{
		regions t_regions;
		t_regions.name = my_import.region_list[i].name;		

		for (int j = 0; j < my_import.region_list[i].perms_list.size(); j++)
		{
			permutations t_perm;
			t_perm.name = my_import.region_list[i].perms_list[j].name;

			for (int k = 0; k < 6; k++)//there are 6 LOD
			{
				int t_size = my_import.region_list[i].perms_list[j].model_files.size();
				if (k < t_size)
				{					
					const aiScene* import_model = aiImportFile(my_import.region_list[i].perms_list[j].model_files[k].c_str(), aiProcessPreset_TargetRealtime_MaxQuality);
					//adding node data ----ONCE PER RENDER_MODEL------
					const aiNode* root_node = import_model->mRootNode;
					if (i == 0 && j == 0 && k == 0)
					{
						for (int l = 0; l < import_model->mNumMeshes; l++)
						{
							aiMesh* t_mesh = import_model->mMeshes[l];
							for (int m = 0; m < t_mesh->mNumBones; m++)
							{
								aiBone* t_bone = t_mesh->mBones[m];
								aiNode* t_ainode = import_model->mRootNode->FindNode(t_bone->mName);

								nodes t_node = { "",0 };
								t_node.name = t_bone->mName.C_Str();
								t_node.parentNode = -1;
								t_node.firstChildNode = -1;
								t_node.nextSiblingNode = -1;
								t_node.importNodeIndex = m;

								aiMatrix4x4* t_mat = new aiMatrix4x4(t_ainode->mTransformation);

								aiVector3D t_ainode_pos;
								aiQuaternion t_ainode_rot;

								t_mat->DecomposeNoScaling(t_ainode_rot, t_ainode_pos);

								t_node.defaultTranslation.x = t_ainode_pos.x / 100;
								t_node.defaultTranslation.y = t_ainode_pos.y / 100;
								t_node.defaultTranslation.z = t_ainode_pos.z / 100;

								t_node.defaultRotation.i = t_ainode_rot.x;
								t_node.defaultRotation.j = t_ainode_rot.y;
								t_node.defaultRotation.k = t_ainode_rot.z;
								t_node.defaultRotation.w = t_ainode_rot.w;

								t_node.distanceFromParent = t_ainode_pos.Length() / 100;

								aiNode* t_parent = t_ainode->mParent;
								//aiMultiplyMatrix4(t_mat, &t_bone->mOffsetMatrix);
								while (t_parent != root_node)
								{
									aiMultiplyMatrix4(t_mat, &t_parent->mTransformation);
									t_parent = t_parent->mParent;
								}

								t_mat->Inverse();//turning it into an inverse

								//i dont know why h2 saves inverses in this manner(maybe just to mislead modders)
								t_node.inverseForward.x = 1;
								t_node.inverseForward.y = t_mat->a1;
								t_node.inverseForward.z = t_mat->b1;

								t_node.inverseLeft.x = t_mat->c1;
								t_node.inverseLeft.y = t_mat->a2;
								t_node.inverseLeft.z = t_mat->b2;

								t_node.inverseUp.x = t_mat->c2;
								t_node.inverseUp.y = t_mat->a3;
								t_node.inverseUp.z = t_mat->b3;

								t_node.inversePosition.x = t_mat->c3;
								t_node.inversePosition.y = t_mat->a4 / 100;
								t_node.inversePosition.z = t_mat->b4 / 100;
								t_node.inverseScale = t_mat->c4 / 100;

								nodes_list.push_back(t_node);
							}
						}
						//linking parent and children
						for (int l = 0; l < nodes_list.size(); l++)
						{
							aiNode* t_ainode = import_model->mRootNode->FindNode(nodes_list[l].name.c_str());

							//adding parent
							if (t_ainode->mParent&&t_ainode->mParent != import_model->mRootNode)
							{
								//parent should be added before children
								for (int m = 0; m < l; m++)
								{
									if (strcmp(t_ainode->mParent->mName.C_Str(), nodes_list[m].name.c_str()) == 0)
									{
										nodes_list[l].parentNode = (short)m;
										break;
									}
								}
							}
							//adding child
							if (t_ainode->mNumChildren)
							{
								int first_child_index = -1;
								for (int m = l + 1; m < nodes_list.size(); m++)
								{
									//for any no of children
									if (t_ainode->mNumChildren > 1)
									{
										for (int a = 0; a < t_ainode->mNumChildren; a++)
										{
											if (!strcmp(t_ainode->mChildren[a]->mName.C_Str(), nodes_list[m].name.c_str()))
											{
												if (first_child_index == -1)
												{
													first_child_index = m;
													nodes_list[l].firstChildNode = first_child_index;
												}
												else
												{
													nodes_list[first_child_index].nextSiblingNode = m;
													first_child_index = m;
												}
											}
										}
									}
									else
									{
										if (!strcmp(t_ainode->mChildren[0]->mName.C_Str(), nodes_list[m].name.c_str()))
										{
											nodes_list[l].firstChildNode = m;
											break;
										}
									}
								}
							}
						}
					}

					section_data t_section(import_model, material_start_index, nodes_list);
					section_data_list.push_back(t_section);

					//all should have same material libraries(same materials) same as the LOD1
					if (k == 0)
					{
						for (int l = 0; l < import_model->mNumMaterials; l++)
						{
							material t_mat;
							t_mat.shader = my_import.region_list[i].perms_list[j].name + "_material_" + to_string(l);
							material_list.push_back(t_mat);
						}
						material_start_index += import_model->mNumMaterials;
					}
					((__int8*)&t_perm.L1)[k] = section_index++;
				}
				else
				{
					((__int8*)&t_perm.L1)[k] = section_index - 1;//using the previous section to fill empty LODs
				}
			}
			t_regions.perms.push_back(t_perm);
		}
		region_list.push_back(t_regions);
	}
}


#pragma endregion definition of functions that load from other file formats
