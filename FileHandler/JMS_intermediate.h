#pragma once

#include<vector>
#include"jms.h"
#include"4x3_transform.h"

using namespace std;
//a struct to encompass intermediate section generated from a JMS file
struct JMS_section_intermediate
{
public:
	int section_pallete_index;
	int material_pallete_index;

	vector<int> vertex_ref_list;
	vector<jms::vertex> t_vertex_list;
	vector<jms::triangle> t_triangle_list;

	JMS_section_intermediate(int, int, int);
	JMS_section_intermediate& operator=(const JMS_section_intermediate& other);
	JMS_section_intermediate(const  JMS_section_intermediate& other);

	void _add_triangle(const jms::triangle, const vector<jms::vertex>&);
private:
	int _get_new_vertex_index(int, const vector<jms::vertex>&);
};
//a kind of link list to create a proper node hierarchy from jms node data
struct JMS_node_intermediate
{
public:
	std::string name;

	real_point3d position;//translation wrt to absolute coordinate
	real_quaternion rotation;//rotation wrt to absolute coordinate

	vector<JMS_node_intermediate*> child_nodes;
	JMS_node_intermediate* parent_node;

	JMS_node_intermediate(std::string name, real_point3d pos, real_quaternion rot, JMS_node_intermediate* parent);
	JMS_node_intermediate& operator=(const JMS_node_intermediate& other);
	JMS_node_intermediate(const JMS_node_intermediate& other);
	~JMS_node_intermediate();

	void _add_child_node(JMS_node_intermediate* child);
	void _get_child_node_by_layer(vector<JMS_node_intermediate*>& out, int layer);
	JMS_node_intermediate* _get_child_node_by_name(std::string);
};