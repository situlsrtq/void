#ifndef TEST_TYPES_H
#define TEST_TYPES_H

#include <string.h>
#include <math.h>

#define VEC3_F -1000
#define VEC4_F -1001
#define MAT4_F -1002
#define R_AXIS_X -1003
#define R_AXIS_Y -1004
#define R_AXIS_Z -1005

#define RADIAN 0.0174532924f

namespace uMATH
{


struct vec3f_t
{
	float x, y, z;
};

struct vec4f_t
{
	float x, y, z, w;
};

struct mat4f_t
{
	float m[4][4];
};


inline vec3f_t Normalize(vec3f_t &v)
{
	float len = sqrtf((v.x * v.x) + (v.y * v.y) + (v.z * v.z));
	v.x /= len;
	v.y /= len;
	v.z /= len;

	return v;
}

inline void SetTransform(mat4f_t *t)
{
	memset(t->m, 0, sizeof(float) * 16);
	t->m[0][0] = 1;
	t->m[1][1] = 1;
	t->m[2][2] = 1;
	t->m[3][3] = 1;
}

inline void SetFrustumHFOV(mat4f_t *t, float fov, float aratio, float near, float far)
{
	float ftan = tanf(fov / 2.0f * RADIAN);
	float right = near * ftan;
	float top = right / aratio;

	t->m[0][0] = near / right;
	t->m[1][1] = near / top;
	t->m[2][2] = -(far + near) / (far - near);
	t->m[2][3] = -(2.0f * far * near) / (far - near);
	t->m[3][2] = -1.0f;
	t->m[3][3] = 0.0f;		// In case this matrix was previously a Transform matrix
}

inline void EulerRotate(mat4f_t *t, float theta, int axis)
{
	if(axis == R_AXIS_Z)
	{
		t->m[0][0] = cosf(theta);
		t->m[0][1] = -sinf(theta);
		t->m[1][0] = sinf(theta);
		t->m[1][1] = cosf(theta);
	}
	else if(axis == R_AXIS_Y)
	{
		t->m[0][0] = cosf(theta);
		t->m[0][2] = -sinf(theta);
		t->m[2][0] = sinf(theta);
		t->m[2][2] = cosf(theta);
	}
	else
	{
		t->m[1][1] = cosf(theta);
		t->m[1][2] = -sinf(theta);
		t->m[2][1] = sinf(theta);
		t->m[2][2] = cosf(theta);
	}
}

inline void MatrixRotate(mat4f_t* t, float d, vec3f_t& r)
{
	float theta = d * RADIAN;
	vec3f_t temp = Normalize(r);

	float c = cosf(theta);
	float s = sinf(theta);
	float vs = (1.0f - cosf(theta));
	float x2 = temp.x * temp.x;
	float y2 = temp.y * temp.y;
	float z2 = temp.z * temp.z;

	t->m[0][0] = (vs * x2) + c;
	t->m[0][1] = (vs * temp.x * temp.y) - (s * temp.z);
	t->m[0][2] = (vs * temp.x * temp.z) + (s * temp.y);

	t->m[1][0] = (vs * temp.x * temp.y) + (s * temp.z);
	t->m[1][1] = (vs * y2) + c;
	t->m[1][2] = (vs * temp.y * temp.z) - (s * temp.x);

	t->m[2][0] = (vs * temp.x * temp.z) - (s * temp.y);
	t->m[2][1] = (vs * temp.y * temp.z) + (s * temp.x);
	t->m[2][2] = (vs * z2) + c;
}

inline void Scale(mat4f_t *t, vec3f_t &s)
{
	t->m[0][0] *= s.x;
	t->m[1][1] *= s.y;
	t->m[2][2] *= s.z;
}

inline void Translate(mat4f_t *t, vec3f_t &s)
{
	t->m[0][3] += s.x;
	t->m[1][3] += s.y;
	t->m[2][3] += s.z;
}

}

#endif
