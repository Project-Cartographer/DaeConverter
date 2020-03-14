#include"JMS_intermediate.h"

JMS_section_intermediate::JMS_section_intermediate(int section_pallete_index, int material_pallete_index, int total_vertex_count) :section_pallete_index(section_pallete_index), material_pallete_index(material_pallete_index)
{
	vertex_ref_list.resize(total_vertex_count, -1);
}
JMS_section_intermediate& JMS_section_intermediate::operator=(const JMS_section_intermediate& other)
{
	section_pallete_index = other.section_pallete_index;
	material_pallete_index = other.material_pallete_index;

	vertex_ref_list = other.vertex_ref_list;
	t_vertex_list = other.t_vertex_list;
	t_triangle_list = other.t_triangle_list;

	return *this;
}
JMS_section_intermediate::JMS_section_intermediate(const JMS_section_intermediate& other)
{
	section_pallete_index = other.section_pallete_index;
	material_pallete_index = other.material_pallete_index;

	vertex_ref_list = other.vertex_ref_list;
	t_vertex_list = other.t_vertex_list;
	t_triangle_list = other.t_triangle_list;
}

void JMS_section_intermediate::_add_triangle(const jms::triangle triangle, const vector<jms::vertex>& jms_vertex_list)
{
	jms::triangle t_triangle;
	t_triangle.vertex_indices.resize(3);

	t_triangle.vertex_indices[0] = _get_new_vertex_index(triangle.vertex_indices[0], jms_vertex_list);
	t_triangle.vertex_indices[1] = _get_new_vertex_index(triangle.vertex_indices[1], jms_vertex_list);
	t_triangle.vertex_indices[2] = _get_new_vertex_index(triangle.vertex_indices[2], jms_vertex_list);

	t_triangle_list.push_back(t_triangle);
}
int JMS_section_intermediate::_get_new_vertex_index(int old_vertex_index, const vector<jms::vertex>& jms_vertex_list)
{
	int ret = -1;
	ret = vertex_ref_list[old_vertex_index];
	if (ret == -1)
	{
		vertex_ref_list[old_vertex_index] = ret = t_vertex_list.size();
		t_vertex_list.push_back(jms_vertex_list[old_vertex_index]);
	}
	return ret;
}

JMS_node_intermediate::JMS_node_intermediate(std::string name, real_point3d pos, real_quaternion rot, JMS_node_intermediate* parent) :
	name(name),
	position(pos),
	rotation(rot),
	parent_node(parent) {}
JMS_node_intermediate& JMS_node_intermediate::operator=(const JMS_node_intermediate& other)
{
	name = other.name;
	position = other.position;
	rotation = other.rotation;
	parent_node = other.parent_node;
	
	return *this;
}
JMS_node_intermediate::JMS_node_intermediate(const JMS_node_intermediate& other)
{
	name = other.name;
	position = other.position;
	rotation = other.rotation;
	parent_node = other.parent_node;
}
JMS_node_intermediate::~JMS_node_intermediate()
{
	for (int i = 0; i < child_nodes.size(); i++)
		delete child_nodes[i];
	child_nodes.clear();
}
void JMS_node_intermediate::_add_child_node(JMS_node_intermediate* child)
{
	vector<JMS_node_intermediate*>::iterator child_iter = child_nodes.begin();

	while (child_iter != child_nodes.end())
	{
		int t_res = child->name.compare((*child_iter)->name);
		if (t_res == 0)
			return;
		if (t_res < 0)
		{
			child_nodes.insert(child_iter, child);
			return;
		}

		child_iter++;
	}
	child_nodes.push_back(child);
}
void JMS_node_intermediate::_get_child_node_by_layer(vector<JMS_node_intermediate*>& out, int layer = 0)
{
	if (!layer)
	{
		out.insert(out.end(), child_nodes.begin(), child_nodes.end());
	}
	else if (layer > 0)
	{
		layer = layer - 1;
		for (int i = 0; i < child_nodes.size(); i++)
			child_nodes[i]->_get_child_node_by_layer(out, layer);
	}
}
JMS_node_intermediate* JMS_node_intermediate::_get_child_node_by_name(std::string name)
{
	//child nodes existence check
	if (!child_nodes.size())
		return nullptr;

	//searches within child
	for (auto i = child_nodes.begin(); i < child_nodes.end(); i++)
	{
		if ((*i)->name.compare(name) == 0)
			return (*i);
	}
	//search within subsequent layers
	for (auto i = child_nodes.begin(); i < child_nodes.end(); i++)
	{
		auto t_ret = (*i)->_get_child_node_by_name(name);

		if (t_ret != nullptr)
			return t_ret;
	}
	//still not found -_-
	return nullptr;
}