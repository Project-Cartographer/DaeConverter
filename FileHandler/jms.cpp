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
			_parse_next(jms_stream, name);
			_parse_next(jms_stream, tif_path);
			break;
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
			node t_node;
			t_node.read(jms_stream, version);
			node_list.push_back(t_node);
		}
		_parse_next(jms_stream, n);//material count
		while (n--)
		{
			material t_mat;
			t_mat.read(jms_stream, version);
			material_list.push_back(t_mat);
		}
		_parse_next(jms_stream, n);//marker count
		while (n--)
		{
			marker t_mark;
			t_mark.read(jms_stream, version);
			marker_list.push_back(t_mark);
		}
		_parse_next(jms_stream, n);//region count
		while (n--)
		{
			region t_reg;
			t_reg.read(jms_stream, version);
			region_list.push_back(t_reg);
		}
		_parse_next(jms_stream, n);//vertex count
		while (n--)
		{
			vertex t_vertex;
			t_vertex.read(jms_stream, version);
			vertex_list.push_back(t_vertex);
		}
		_parse_next(jms_stream, n);//triangle list
		while (n--)
		{
			triangle t_tri;
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
			node t_node;
			t_node.read(jms_stream, version);
			node_list.push_back(t_node);
		}
		_parse_next(jms_stream, n);//material count
		while (n--)
		{
			material t_mat;
			t_mat.read(jms_stream, version);
			material_list.push_back(t_mat);
		}
		_parse_next(jms_stream, n);//marker count
		while (n--)
		{
			marker t_mark;
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
			vertex t_vertex;
			t_vertex.read(jms_stream, version);
			vertex_list.push_back(t_vertex);
		}
		_parse_next(jms_stream, n);//triangle list
		while (n--)
		{
			triangle t_tri;
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
}