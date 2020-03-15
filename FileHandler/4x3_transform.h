#include<math.h>
#include<algorithm>

#pragma once
/*
-Bunch of functions to generate a 4x3 matrix and perform vector multiplications onto them
-4x3 transform matrix generation program taken from https://github.com/Project-Cartographer/H2Codez/blob/ec498bc03f5f08cd0ff15786b0da3cd7b3539925/H2Codez/Common/BlamBaseTypes.h#L515
-Decomposition method taken from https://github.com/ElDewrito/ElDorito/blob/a44c46d6edbfd4a54e314388a34e43c6e2363ddb/ElDorito/Source/Blam/Math/RealQuaternion.cpp#L198
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
	void get_inverse(real_quaternion& out)
	{
		real_quaternion conj_q;
		get_conjugate(conj_q);

		auto square_length = get_square_length();

		out.i = conj_q.i / square_length;
		out.j = conj_q.j / square_length;
		out.k = conj_q.k / square_length;
		out.w = conj_q.w / square_length;
	}
	void get_conjugate(real_quaternion& out)
	{
		out.i = -i;
		out.j = -j;
		out.k = -k;
		out.w = w;
	}
	//a*b multiplication operation
	static void multiply(real_quaternion& out, const real_quaternion& a, const real_quaternion& b)
	{
		float q1x = a.i;
		float q1y = a.j;
		float q1z = a.k;
		float q1w = a.w;

		float q2x = a.i;
		float q2y = a.j;
		float q2z = a.j;
		float q2w = a.w;

		// cross(av, bv)
		float cx = q1y * q2z - q1z * q2y;
		float cy = q1z * q2x - q1x * q2z;
		float cz = q1x * q2y - q1y * q2x;

		float dot = q1x * q2x + q1y * q2y + q1z * q2z;

		out.i = q1x * q2w + q2x * q1w + cx;
		out.j = q1y * q2w + q2y * q1w + cy;
		out.k = q1z * q2w + q2z * q1w + cz;
		out.w = q1w * q2w - dot;
	}
};
struct real_matrix4x3
{
	float scale = 1.0f;
	real_vector3d forward;
	real_vector3d left;
	real_vector3d up;
	real_point3d translation;

	real_matrix4x3()
	{
		forward = left = up =  { 0 };
		translation = { 0 };
	}
	real_matrix4x3(const real_quaternion& rotation)
	{
		translation = { 0 };
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
	void transform_vector(real_point3d vec)
	{
		vec.x = vec.x * forward.i + vec.y * left.i + vec.z * up.i + translation.x;
		vec.y = vec.x * forward.j + vec.y * left.j + vec.z * up.j + translation.y;
		vec.z = vec.x * forward.k + vec.y * left.k + vec.z * up.k + translation.z;
	}
	//a*b multiplication operation
	static void multiply(real_matrix4x3& out, const real_matrix4x3& a, const real_matrix4x3& b)
	{
		float f1i = a.forward.i;
		float f1j = a.forward.j;
		float f1k = a.forward.k;
		
		float l1i = a.left.i;
		float l1j = a.left.j;
		float l1k = a.left.k;

		float u1i = a.up.i;
		float u1j = a.up.j;
		float u1k = a.up.k;

		float p1x = a.translation.x;
		float p1y = a.translation.y;
		float p1z = a.translation.z;

		float f2i = b.forward.i;
		float f2j = b.forward.j;
		float f2k = b.forward.k;

		float l2i = b.left.i;
		float l2j = b.left.j;
		float l2k = b.left.k;

		float u2i = b.up.i;
		float u2j = b.up.j;
		float u2k = b.up.k;

		float p2x = b.translation.x;
		float p2y = b.translation.y;
		float p2z = b.translation.z;

		out.forward.i = f1i*f2i + f1j*l2i + f1k*u2i;
		out.forward.j = f1i*f2j + f1j*l2j + f1k*u2j;
		out.forward.k = f1i*f2k + f1j*l2k + f1k*u2k;

		out.left.i = l1i*f2i + l1j*l2i + l1k*u2i;
		out.left.j = l1i*f2j + l1j*l2j + l1k*u2j;
		out.left.k = l1i*f2k + l1j*l2k + l1k*u2k;

		out.up.i = u1i*f2i + u1j*l2i + u1k*u2i;
		out.up.j = u1i*f2j + u1j*l2j + u1k*u2j;
		out.up.k = u1i*f2k + u1j*l2k + u1k*u2k;

		out.translation.x = p1x*f2i + p1y*l2i + p1z*u2i + 1 * p2x;
		out.translation.y = p1x*f2j + p1y*l2j + p1z*u2j + 1 * p2y;
		out.translation.z = p1x*f2k + p1y*l2k + p1z*u2k + 1 * p2z;
	}
	void decompose_matrix(real_point3d& pos_out,real_quaternion& rot_out)
	{
		pos_out = translation;

		float trace = forward.i + left.j + up.k;

		if (trace > 0.0f)
		{
			float s = (float)std::sqrt(trace + 1.0f);
			rot_out.w = s * 0.5f;
			s = 0.5f / s;
			rot_out.i = (left.k - up.j) * s;
			rot_out.j = (up.i -forward.k) * s;
			rot_out.k = (forward.j - left.i) * s;
		}
		else
		{
			if (forward.i >= left.j && forward.i >= up.k)
			{
				float s = (float)std::sqrt(1.0f + forward.i - left.j - up.k);
				float invS = 0.5f / s;
				rot_out.i = 0.5f * s;
				rot_out.j = (forward.j + left.i) * invS;
				rot_out.k = (forward.k + up.i) * invS;
				rot_out.w = (left.k - up.j) * invS;
			}
			else if (left.j > up.k)
			{
				float s = (float)std::sqrt(1.0f + left.j - forward.i - up.k);
				float invS = 0.5f / s;
				rot_out.i = (left.i + forward.j) * invS;
				rot_out.j = 0.5f * s;
				rot_out.k = (up.j + left.k) * invS;
				rot_out.w = (up.i - forward.k) * invS;
			}
			else
			{
				float s = (float)std::sqrt(1.0f + up.k - forward.i - left.j);
				float invS = 0.5f / s;
				rot_out.i = (up.i + forward.k) * invS;
				rot_out.j = (up.j + left.k) * invS;
				rot_out.k = 0.5f * s;
				rot_out.w = (forward.j - left.i) * invS;
			}
		}
	}
};