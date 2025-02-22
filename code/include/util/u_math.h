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

inline void SetTransform(mat4f_t *t)
{
	memset(t->m, 0, sizeof(float) * 16);
	t->m[0][0] = 1;
	t->m[1][1] = 1;
	t->m[2][2] = 1;
	t->m[3][3] = 1;
}

inline void Rotate(mat4f_t *t, float theta, int axis)
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

inline void Scale(mat4f_t *t, vec3f_t *s)
{
	t->m[0][0] *= s->x;
	t->m[1][1] *= s->y;
	t->m[2][2] *= s->z;
}

inline void Translate(mat4f_t *t, vec3f_t *s)
{
	t->m[0][3] += s->x;
	t->m[1][3] += s->y;
	t->m[2][3] += s->z;
}

#endif
