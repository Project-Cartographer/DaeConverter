#pragma once
#include<vector>

///
//A structure encompass different attributes of a triangular face
///
struct triangle_face
{
	///v0,v1,v2
	std::vector<int> vertex_list;
	///
	//A structure to implement attributes of an edge of a triangle
	///
	struct triangle_edge
	{
		int v0;
		int v1;
		int adjacent_index;
		///default ctor
		triangle_edge() {
			v0 = v1 = adjacent_index = -1;
		}
		///a better ctor
		triangle_edge(int v0,int v1,int adjacent_index):v0(v0),v1(v1),adjacent_index(adjacent_index)
		{}
		///copy ctor
		triangle_edge(const triangle_edge &other):v0(other.v0),v1(other.v1),adjacent_index(other.adjacent_index)
		{}
		///assignment operator
		triangle_edge& operator=(const triangle_edge &other)
		{
			v0 = other.v0;
			v1 = other.v1;
			adjacent_index = other.adjacent_index;

			return *this;
		}
	};
	///[v0,v1],[v1,v2],[v2,v0]
	std::vector<triangle_edge> adjacency_list;
	///default ctor
	triangle_face(){
		vertex_list.resize(3, -1);
		adjacency_list.resize(3, triangle_edge());
	}
	///a better ctor
	triangle_face(int v0, int v1, int v2)
	{
		triangle_face();
		vertex_list[0] = v0;
		vertex_list[1] = v1;
		vertex_list[2] = v2;

		adjacency_list[0] = { v0,v1,-1 };
		adjacency_list[1] = { v1,v2,-1 };
		adjacency_list[2] = { v2,v0,-1 };
	}
	///copy ctor
	triangle_face(const triangle_face &other)
	{
		vertex_list = other.vertex_list;
		adjacency_list = other.adjacency_list;
	}
	///assgnment operator
	triangle_face& operator=(const triangle_face &other)
	{
		vertex_list = other.vertex_list;
		adjacency_list = other.adjacency_list;

		return *this;
	}
	///returns the edge index that is common in both the triangles
	///and assigns the triangle index to itself
	int get_matching_edge(const triangle_face &other,int other_index)
	{

	}
};
