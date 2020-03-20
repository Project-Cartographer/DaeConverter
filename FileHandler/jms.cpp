#include "jms.h"
#include<queue>

namespace jms
{
	#define _buffer_length 128

	std::queue<std::string> arg_list;
	const char *whitespace = " \t\r\n\f";
	char buffer[_buffer_length] = "";

	void _parse_next(std::ifstream* in, std::string& out)
	{	
		//if have tokens
		if (arg_list.size())
		{
			out = arg_list.front();
			arg_list.pop();
			return;
		}
		if (in->eof())
		{
			std::cout << "\nError reading ,End Of File reached";
			return;
		}
		std::string temp;
		in->getline(buffer, _buffer_length);
		buffer[(std::find(buffer, buffer + strlen(buffer), ';') - buffer)] = '\0';
		char* t = strtok(buffer, whitespace);
		while (t)
		{
			temp = t;
			arg_list.push(temp);
			t = strtok(nullptr, whitespace);
		}
		_parse_next(in, out);		
	}
	void _parse_next(std::ifstream* in, int& out)
	{
		std::string temp;
		_parse_next(in, temp);
		out = std::stoi(temp);
	}
	void _parse_next(std::ifstream* in, float& out)
	{
		std::string temp;
		_parse_next(in, temp);
		out = std::stof(temp.c_str());
	}

	//read from stream
	void vector3D::read(std::ifstream* jms_stream,int version)
	{		
		_parse_next(jms_stream, x);
		_parse_next(jms_stream, y);
		_parse_next(jms_stream, z);
	}
	//single line declaration on current line
	void vector3D::write(std::ofstream* jms_stream,int version)
	{
		*jms_stream << x << '\t';
		*jms_stream << y << '\t';
		*jms_stream << z << '\n';
	}
	//read from stream
	void vector4D::read(std::ifstream* jms_stream,int version)
	{
		_parse_next(jms_stream, x);
		_parse_next(jms_stream, y);
		_parse_next(jms_stream, z);
		_parse_next(jms_stream, w);
	}
	//single line declaration on current line
	void vector4D::write(std::ofstream* jms_stream,int version)
	{
		*jms_stream << x << '\t';
		*jms_stream << y << '\t';
		*jms_stream << z << '\t';
		*jms_stream << w << '\n';
	}
	//read from stream
	void node::read(std::ifstream* jms_stream,int version)
	{
		switch (version)
		{
		case 8200:
			_parse_next(jms_stream, name);
			_parse_next(jms_stream, first_child_node_index);
			_parse_next(jms_stream, sibling_node_index);
			rotation.read(jms_stream, version);
			position.read(jms_stream, version);
			break;
		case 8210:
			_parse_next(jms_stream, name);
			_parse_next(jms_stream, parent_node_index);
			rotation.read(jms_stream, version);
			position.read(jms_stream, version);
			break;
		default: std::cout << "\nUnable to read node data,unsupported version : " << version;
		}
		//do some cleanup for node names
		int pos = name.find("frame_");
		if (pos != std::string::npos)
			name.erase(pos, 6);
		pos = name.find("b_");
		if (pos != std::string::npos)
			name.erase(pos, 2);
	}
	//write to stream
	void node::write(std::ofstream* jms_stream, int version)
	{
		switch (version)
		{
		case 8200:
			*jms_stream << name << '\n';
			*jms_stream << first_child_node_index << '\n';
			*jms_stream << sibling_node_index << '\n';
			rotation.write(jms_stream, version);
			position.write(jms_stream, version);
			break;
		case 8210:
			*jms_stream << name << '\n';
			*jms_stream << parent_node_index << '\n';
			rotation.write(jms_stream, version);
			position.write(jms_stream, version);
			break;
		default: std::cout << "\nUnable to write node data,unsupported version : " << version;
		}
	}
	//read from stream
	void material::read(std::ifstream* jms_stream,int version)
	{
		switch (version)
		{
		case 8200:
			//_parse_next(jms_stream, name);
			//_parse_next(jms_stream, tif_path);
			//break;
		case 8210:
			_parse_next(jms_stream, name);
			_parse_next(jms_stream, LOD);
			_parse_next(jms_stream, Permutation);
			_parse_next(jms_stream, Region);
			break;
		default:;
		}
	}
	//write to stream
	void material::write(std::ofstream* jms_stream, int version)
	{
		switch (version)
		{
		case 8200:
			*jms_stream << name << '\n';
			*jms_stream << tif_path << '\n';
			break;
		case 8210:
			*jms_stream << name << '\n';
			*jms_stream << LOD << '\t' << Permutation << '\t' << Region << '\n';
			break;
		default:;
		}

	}
	//read from stream
	void marker::read(std::ifstream* jms_stream, int version)
	{
		switch (version)
		{
		case 8200:
			_parse_next(jms_stream, name);
			_parse_next(jms_stream, region);
			_parse_next(jms_stream, parent_node);
			rotation.read(jms_stream, version);
			position.read(jms_stream, version);
			_parse_next(jms_stream, radius);
			break;
		case 8210:
			_parse_next(jms_stream, name);
			_parse_next(jms_stream, parent_node);
			rotation.read(jms_stream, version);
			position.read(jms_stream, version);
			_parse_next(jms_stream, radius);
			break;
		default: std::cout << "\nUnable to read marker data,unsupported version : " << version;
		}
	}
	//write to stream
	void marker::write(std::ofstream* jms_stream, int version)
	{
		switch (version)
		{
		case 8200:
			*jms_stream << name << '\n';
			*jms_stream << region << '\n';
			*jms_stream << parent_node << '\n';
			rotation.write(jms_stream, version);
			position.write(jms_stream, version);
			*jms_stream << radius << '\n';
			break;
		case 8210:
			*jms_stream << name << '\n';
			*jms_stream << parent_node << '\n';
			rotation.write(jms_stream, version);
			position.write(jms_stream, version);
			*jms_stream << radius << '\n';
			break;
		default: std::cout << "\nUnable to write marker data,unsupported version : " << version;
		}
	}
	//read from stream
	void region::read(std::ifstream* jms_stream, int version)
	{
		_parse_next(jms_stream, name);
	}
	//write to stream
	void region::write(std::ofstream* jms_stream, int version)
	{
		*jms_stream << name << '\n';
	}
	//read from stream
	void vertex::read(std::ifstream* jms_stream, int version)
	{
		int t;
		float f;
		switch (version)
		{
		case 8200://2 nodes MAX lol
			_parse_next(jms_stream, t);
			node_indices.push_back(t);
			position.read(jms_stream, version);
			normal.read(jms_stream, version);
			_parse_next(jms_stream, t);
			_parse_next(jms_stream, f);
			node_weights.push_back(1 - f);
			if (t != -1)
			{
				node_indices.push_back(t);
				node_weights.push_back(f);
			}
			tex_cords.read(jms_stream, version);
			break;
		case 8210:
			int n_count, t_count;
			position.read(jms_stream, version);
			normal.read(jms_stream, version);
			_parse_next(jms_stream, n_count);
			while (n_count--)
			{
				_parse_next(jms_stream, t);
				_parse_next(jms_stream, f);
				node_indices.push_back(t);
				node_weights.push_back(f);
			}
			_parse_next(jms_stream, t_count);
			//i support only one coordinate :P
			while (t_count--)
			{   //x,y reading
				_parse_next(jms_stream, f);
				tex_cords.x = f;
				_parse_next(jms_stream, f);
				tex_cords.y = f;
				tex_cords.z = 0.0;
			}
			break;
		default: std::cout << "\nUnable to read vertex data,Unsupported version : " << version;
		}
	}
	//write to stream
	void vertex::write(std::ofstream* jms_stream, int version)
	{
		switch (version)
		{
		case 8200:
			*jms_stream << node_indices[0] << '\n';
			position.write(jms_stream, version);
			normal.write(jms_stream, version);
			if (node_indices.size() > 1)
			{
				*jms_stream << node_indices[1] << '\t';
				*jms_stream << node_weights[1] << '\n';
			}
			else
			{
				*jms_stream << -1 << '\t';
				*jms_stream << 0 << '\n';
			}
			tex_cords.write(jms_stream, version);
			break;
		case 8210:
			position.write(jms_stream, version);
			normal.write(jms_stream, version);
			*jms_stream << node_indices.size() << '\n';
			for (int i = 0; i < node_indices.size(); i++)
				*jms_stream << node_indices[i] << '\t' << node_weights[i] << '\n';
			*jms_stream << 1 << '\n';
			*jms_stream << tex_cords.x << '\t' << tex_cords.y << '\n';
			break;
		default: std::cout << "\nUnable to write vertex data,Unsupported version : " << version;
		}
	}
	//read from stream
	void triangle::read(std::ifstream* jms_stream, int version)
	{
		int n,t;
		switch (version)
		{
		case 8200:
			_parse_next(jms_stream, region_index);
			_parse_next(jms_stream, shader_index);
			n = 3;
			while (n--)
			{
				_parse_next(jms_stream, t);
				vertex_indices.push_back(t);
			}
			break;
		case 8210:
			_parse_next(jms_stream, shader_index);
			n = 3;
			while (n--)
			{
				_parse_next(jms_stream, t);
				vertex_indices.push_back(t);
			}
			break;
		default:;
		}
	}
	//write to stream
	void triangle::write(std::ofstream* jms_stream, int version)
	{
		switch (version)
		{
		case 8200:
			*jms_stream << region_index << '\n';
			*jms_stream << shader_index << '\n';
			*jms_stream << vertex_indices[0] << '\t' << vertex_indices[1] << '\t' << vertex_indices[2] << '\n';
			break;
		case 8210:
			*jms_stream << shader_index << '\n';
			*jms_stream << vertex_indices[0] << '\t' << vertex_indices[1] << '\t' << vertex_indices[2] << '\n';
			break;
		default:;
		}
	}
	jms::jms()
	{
		version = 8210;
		checksum = 0;
	}
	jms::jms(int arg0)
	{
		version = arg0;
		checksum = 0;
	}
	jms::jms(std::string file_loc)
	{
		std::ifstream* jms_stream = new std::ifstream();
		jms_stream->open(file_loc.c_str(), std::ios::in);

		if (jms_stream->is_open())
		{
			_parse_next(jms_stream, version);
			switch (version)
			{
			case 8200:
				read_jms8200(jms_stream);
				fix_jms8200_parent_ref();
				break;
			case 8210:
				read_jms8210(jms_stream);
				break;
			default:
				std::cout << "\nUnsupported version : " << version;
				break;
			}
		}
		else
		{
			std::cout << "\nFile doesnt exist : " << file_loc;
		}

		clean_redundant_vertices();

		jms_stream->close();
		delete jms_stream;
	}
	void jms::save_file(std::string file_loc)
	{
		std::ofstream* jms_stream = new std::ofstream();
		jms_stream->open(file_loc, std::ios::out);

		*jms_stream << version << '\n';
		switch (version)
		{
		case 8200:
			write_jms8200(jms_stream);
			break;
		case 8210:
			write_jms8210(jms_stream);
			break;
		default:
			std::cout << "\nUnsupported version : " << version;
			break;
		}
		jms_stream->close();
		delete jms_stream;
	}
	void jms::read_jms8200(std::ifstream* jms_stream)
	{
		int n;
		_parse_next(jms_stream, checksum);
		_parse_next(jms_stream, n);//node count
		while (n--)
		{
			node t_node = node();
			t_node.read(jms_stream, version);
			node_list.push_back(t_node);
		}
		_parse_next(jms_stream, n);//material count
		while (n--)
		{
			material t_mat = material();
			t_mat.read(jms_stream, version);
			material_list.push_back(t_mat);
		}
		_parse_next(jms_stream, n);//marker count
		while (n--)
		{
			marker t_mark = marker();
			t_mark.read(jms_stream, version);
			marker_list.push_back(t_mark);
		}
		_parse_next(jms_stream, n);//region count
		while (n--)
		{
			region t_reg = region();
			t_reg.read(jms_stream, version);
			region_list.push_back(t_reg);
		}
		_parse_next(jms_stream, n);//vertex count
		while (n--)
		{
			vertex t_vertex = vertex();
			t_vertex.read(jms_stream, version);
			vertex_list.push_back(t_vertex);
		}
		_parse_next(jms_stream, n);//triangle list
		while (n--)
		{
			triangle t_tri = triangle();
			t_tri.read(jms_stream, version);
			triangle_list.push_back(t_tri);
		}
	}
	void jms::read_jms8210(std::ifstream* jms_stream)
	{
		int n;
		_parse_next(jms_stream, n);//node count
		while (n--)
		{
			node t_node = node();
			t_node.read(jms_stream, version);
			node_list.push_back(t_node);
		}
		_parse_next(jms_stream, n);//material count
		while (n--)
		{
			material t_mat = material();
			t_mat.read(jms_stream, version);
			material_list.push_back(t_mat);
		}
		_parse_next(jms_stream, n);//marker count
		while (n--)
		{
			marker t_mark = marker();
			t_mark.read(jms_stream, version);
			marker_list.push_back(t_mark);
		}
		_parse_next(jms_stream, n);//INSTANCE XREF PATHS count
		if (n)
		{
			std::cout << "\nFound unsupported field INSTANCE XREF PATHS\nTerminating Parser\n";
			return;
		}
		_parse_next(jms_stream, n);//INSTANCE MARKERS count
		if (n)
		{
			std::cout << "\nFound unsupported field INSTANCE MARKERS\nTerminating Parser\n";
			return;
		}
		_parse_next(jms_stream, n);//vertex count
		while (n--)
		{
			vertex t_vertex = vertex();
			t_vertex.read(jms_stream, version);
			vertex_list.push_back(t_vertex);
		}
		_parse_next(jms_stream, n);//triangle list
		while (n--)
		{
			triangle t_tri = triangle();
			t_tri.read(jms_stream, version);
			triangle_list.push_back(t_tri);
		}
	}
	void jms::write_jms8200(std::ofstream* jms_stream)
	{
		*jms_stream << checksum << '\n';
		*jms_stream << node_list.size() << '\n';
		for (int i = 0; i < node_list.size(); i++)
			node_list[i].write(jms_stream, version);
		*jms_stream << material_list.size() << '\n';
		for (int i = 0; i < material_list.size(); i++)
			material_list[i].write(jms_stream, version);
		*jms_stream << marker_list.size() << '\n';
		for (int i = 0; i < marker_list.size(); i++)
			marker_list[i].write(jms_stream, version);
		*jms_stream << region_list.size() << '\n';
		for (int i = 0; i < region_list.size(); i++)
			region_list[i].write(jms_stream, version);
		*jms_stream << vertex_list.size() << '\n';
		for (int i = 0; i < vertex_list.size(); i++)
			vertex_list[i].write(jms_stream, version);
		*jms_stream << triangle_list.size() << '\n';
		for (int i = 0; i < triangle_list.size(); i++)
			triangle_list[i].write(jms_stream, version);

	}
	void jms::write_jms8210(std::ofstream* jms_stream)
	{
		*jms_stream << node_list.size() << '\n';
		for (int i = 0; i < node_list.size(); i++)
			node_list[i].write(jms_stream, version);
		*jms_stream << material_list.size() << '\n';
		for (int i = 0; i < material_list.size(); i++)
			material_list[i].write(jms_stream, version);
		*jms_stream << marker_list.size() << '\n';
		for (int i = 0; i < marker_list.size(); i++)
			marker_list[i].write(jms_stream, version);
		*jms_stream << 0 << '\n';//INSTANCE XREF PATHS count
		*jms_stream << 0 << '\n';//INSTANCE MARKERS count
		*jms_stream << vertex_list.size() << '\n';
		for (int i = 0; i < vertex_list.size(); i++)
			vertex_list[i].write(jms_stream, version);
		*jms_stream << triangle_list.size() << '\n';
		for (int i = 0; i < triangle_list.size(); i++)
			triangle_list[i].write(jms_stream, version);

	}
	jms::~jms()
	{
	}
	void jms::fix_jms8200_parent_ref()
	{
		//To streamline the line subsequent conversion processes
		///jms 8200 doesnt mention parent node index
		///rather it mentions child node and sibilling indices
		///the subsquent conversion processes utilize parent index for referencing nodes
		for (int i = 0; i < node_list.size(); i++)
		{
			node& current_node = node_list[i];

			///update the parent index of sibillings
			if (current_node.parent_node_index != -1)
			{
				int sibbiling_node_index = current_node.sibling_node_index;
				while (sibbiling_node_index != -1)
				{
					node_list[sibbiling_node_index].parent_node_index = current_node.parent_node_index;
					sibbiling_node_index = node_list[sibbiling_node_index].sibling_node_index;
				}
			}
			///child node
			if (current_node.first_child_node_index != -1)
				node_list[current_node.first_child_node_index].parent_node_index = i;
		}
	}
	//function to remove similar/identical vertices
	///Extra function to do some clean up
	///which some lazy ass export plugin fail to do so
	///resulting in declaration of 3 vertex per triangle
	void jms::clean_redundant_vertices(float threshold)
	{
		///well the same vertex has got different normal depending on the face it is utilized and therefore has to averaged
		std::vector<int> vertex_ref;
		vertex_ref.resize(vertex_list.size(), 0);			///list to store references to newly added or matched up vertices

		std::vector<vertex> new_vertex_list;				///new reduced vertex list
		std::vector<int> vertex_ref_count;					///list to store no instance the vertex has been referred

		for (int vertex_iter = 0; vertex_iter < vertex_list.size(); vertex_iter++)
		{
			vertex& current_vertex = vertex_list[vertex_iter];
			///look for it
			int match_index = -1, new_ver_iter = 0;
			while (new_ver_iter < new_vertex_list.size())
			{
				vertex& comp_vertex = new_vertex_list[new_ver_iter];
				if (abs(comp_vertex.position.x - current_vertex.position.x) < threshold)
				{
					if (abs(comp_vertex.position.y - current_vertex.position.y) < threshold)
					{
						if (abs(comp_vertex.position.z - current_vertex.position.z) < threshold)
						{
							///found our vertex
							match_index = new_ver_iter;
							break;
						}
					}
				}
				new_ver_iter++;
			}
			if (match_index == -1)
			{
				///no match,new vertex
				match_index = new_vertex_list.size();
				new_vertex_list.push_back(current_vertex);				///add the new vertex
				vertex_ref_count.push_back(1);							///add a new entry to refs count(for averaging purpose)
				vertex_ref[vertex_iter] = match_index;					///save the new reference for the vertex in the new list
			}
			else
			{
				///match found !!
				vertex_ref_count[match_index]++;						///increase ref count
				new_vertex_list[match_index].normal = new_vertex_list[match_index].normal + vertex_list[vertex_iter].normal;///for averaging purpose
				vertex_ref[vertex_iter] = match_index;					///save the new reference for the duplicate vertex
			}
		}
		///averaging normals;
		for (int i = 0; i < new_vertex_list.size(); i++)
			new_vertex_list[i].normal = new_vertex_list[i].normal*(1.0f / vertex_ref_count[i]);
		///now assign our new vertex list
		vertex_list.assign(new_vertex_list.begin(), new_vertex_list.end());
		///now to fix the vertex reference present in the triangles
		for (int tri_iter = 0; tri_iter < triangle_list.size(); tri_iter++)
		{
			int v0 = triangle_list[tri_iter].vertex_indices[0];
			int v1 = triangle_list[tri_iter].vertex_indices[1];
			int v2 = triangle_list[tri_iter].vertex_indices[2];

			triangle_list[tri_iter].vertex_indices[0] = vertex_ref[v0];
			triangle_list[tri_iter].vertex_indices[1] = vertex_ref[v1];
			triangle_list[tri_iter].vertex_indices[2] = vertex_ref[v2];
		}
	}
}