#include<math.h>
#include<algorithm>

#pragma once
/*
-Bunch of functions to generate a 4x3 matrix and perform vector multiplications onto them
-4x3 transform matrix generation program taken from https://github.com/Project-Cartographer/H2Codez/blob/ec498bc03f5f08cd0ff15786b0da3cd7b3539925/H2Codez/Common/BlamBaseTypes.h#L515
*/


struct angle
{
	float rad = 0.0f;

	angle() {};

	angle(float _rad) :
		rad(_rad)
	{}

	double as_degree()
	{
		return rad * (180.0 / 3.14159265358979323846);
	}

	double as_rad()
	{
		return rad;
	}
};

struct real_euler_angles3d
{
	angle yaw;
	angle pitch;
	angle roll;
};

struct real_vector3d
{
	float i;
	float j;
	float k;
	real_euler_angles3d get_angle()
	{
		real_euler_angles3d angle;
		angle.yaw = acos(i);
		angle.pitch = acos(j);
		angle.roll = acos(k);
		return angle;
	}
};
struct real_point3d
{
	float x;
	float y;
	float z;
};
struct real_quaternion
{
	float i;
	float j;
	float k;
	float w;

	inline float get_square_length() const
	{
		return i * i + j * j + k * k + w * w;
	}
};
struct real_matrix4x3
{
	float scale = 1.0f;
	real_vector3d forward;
	real_vector3d left;
	real_vector3d up;
	real_point3d translation;

	real_matrix4x3(const real_quaternion& rotation)
	{
		set_rotation(rotation);
	}

	real_matrix4x3(const real_quaternion& _rotation, const real_point3d& _translation):translation(_translation)
	{
		set_rotation(_rotation);
	}

	inline void inverse_rotation()
	{
		std::swap(forward.j, left.i);
		std::swap(forward.k, up.i);
		std::swap(left.k, up.j);
	}

	inline void inverse()
	{
		//assert(scale != 0.0f);

		scale = 1.0f / scale;

		float inverse_pos_x = -translation.x * scale;
		float inverse_pos_y = -translation.y * scale;
		float inverse_pos_z = -translation.z * scale;

		translation.x = (inverse_pos_x * forward.i) + (inverse_pos_y * left.i) + (inverse_pos_z * up.i);
		translation.y = (inverse_pos_x * forward.j) + (inverse_pos_y * left.j) + (inverse_pos_z * up.j);
		translation.z = (inverse_pos_x * forward.k) + (inverse_pos_y * left.k) + (inverse_pos_z * up.k);

		inverse_rotation();
	};

	void set_rotation(const real_quaternion& rotation)
	{
		float square_len = rotation.get_square_length();
		//assert(square_len != 0.0f);
		float s = 2.0f / square_len;

		auto is = rotation.i * s;
		auto js = rotation.j * s;
		auto ks = rotation.k * s;

		auto iw = rotation.w * is;
		auto jw = rotation.w * js;
		auto kw = rotation.w * ks;

		auto ii = rotation.i * is, jj = rotation.j * js, kk = rotation.k * ks;
		auto ij = rotation.i * js, ik = rotation.i * ks, jk = rotation.j * ks;

		forward = { 1.0f - (jj + kk),  ij - kw,            ik + jw };
		left = { ij + kw,           1.0f - (ii + kk),   jk - iw };
		up = { ik - jw,           jk + iw,            1.0f - (ii + jj) };
	}
};