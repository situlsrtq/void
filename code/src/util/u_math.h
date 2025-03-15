#ifndef MBOX_UMATH_H
#define MBOX_UMATH_H

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


//-------------------------------STRUCTURES-------------------------------


struct vec3f_t
{
	float x, y, z;

	vec3f_t operator-(const vec3f_t& s) const
	{
		vec3f_t r;
		r.x = this->x - s.x;
		r.y = this->y - s.y;
		r.z = this->z - s.z;
		
		return r;
	}
	
	void operator-=(const vec3f_t& s)
	{
		this->x -= s.x;
		this->y -= s.y;
		this->z -= s.z;
	}

	vec3f_t operator+(const vec3f_t& s) const
	{
		vec3f_t r;
		r.x = this->x + s.x;
		r.y = this->y + s.y;
		r.z = this->z + s.z;
		
		return r;
	}
	
	void operator+=(const vec3f_t& s)
	{
		this->x += s.x;
		this->y += s.y;
		this->z += s.z;
	}
};


struct vec4f_t
{
	float x, y, z, w;
};


struct mat4f_t
{
	float m[4][4];

	void operator *=(const mat4f_t &s)
	{
		m[0][0] = (m[0][0] * s.m[0][0]) + (m[0][1] * s.m[1][0]) + (m[0][2] * s.m[2][0]) + (m[0][3] * s.m[3][0]);
		m[0][1] = (m[0][0] * s.m[0][1]) + (m[0][1] * s.m[1][1]) + (m[0][2] * s.m[2][1]) + (m[0][3] * s.m[3][1]);
		m[0][2] = (m[0][0] * s.m[0][2]) + (m[0][1] * s.m[1][2]) + (m[0][2] * s.m[2][2]) + (m[0][3] * s.m[3][2]);
		m[0][3] = (m[0][0] * s.m[0][3]) + (m[0][1] * s.m[1][3]) + (m[0][2] * s.m[2][3]) + (m[0][3] * s.m[3][3]);

		m[1][0] = (m[1][0] * s.m[0][0]) + (m[1][1] * s.m[1][0]) + (m[1][2] * s.m[2][0]) + (m[1][3] * s.m[3][0]);
		m[1][1] = (m[1][0] * s.m[0][1]) + (m[1][1] * s.m[1][1]) + (m[1][2] * s.m[2][1]) + (m[1][3] * s.m[3][1]);
		m[1][2] = (m[1][0] * s.m[0][2]) + (m[1][1] * s.m[1][2]) + (m[1][2] * s.m[2][2]) + (m[1][3] * s.m[3][2]);
		m[1][3] = (m[1][0] * s.m[0][3]) + (m[1][1] * s.m[1][3]) + (m[1][2] * s.m[2][3]) + (m[1][3] * s.m[3][3]);

		m[2][0] = (m[2][0] * s.m[0][0]) + (m[2][1] * s.m[1][0]) + (m[2][2] * s.m[2][0]) + (m[2][3] * s.m[3][0]);
		m[2][1] = (m[2][0] * s.m[0][1]) + (m[2][1] * s.m[1][1]) + (m[2][2] * s.m[2][1]) + (m[2][3] * s.m[3][1]);
		m[2][2] = (m[2][0] * s.m[0][2]) + (m[2][1] * s.m[1][2]) + (m[2][2] * s.m[2][2]) + (m[2][3] * s.m[3][2]);
		m[2][3] = (m[2][0] * s.m[0][3]) + (m[2][1] * s.m[1][3]) + (m[2][2] * s.m[2][3]) + (m[2][3] * s.m[3][3]);

		m[3][0] = (m[3][0] * s.m[0][0]) + (m[3][1] * s.m[1][0]) + (m[3][2] * s.m[2][0]) + (m[3][3] * s.m[3][0]);
		m[3][1] = (m[3][0] * s.m[0][1]) + (m[3][1] * s.m[1][1]) + (m[3][2] * s.m[2][1]) + (m[3][3] * s.m[3][1]);
		m[3][2] = (m[3][0] * s.m[0][2]) + (m[3][1] * s.m[1][2]) + (m[3][2] * s.m[2][2]) + (m[3][3] * s.m[3][2]);
		m[3][3] = (m[3][0] * s.m[0][3]) + (m[3][1] * s.m[1][3]) + (m[3][2] * s.m[2][3]) + (m[3][3] * s.m[3][3]);
	}
};


//-----------------------------------FUNCTIONS---------------------------------------


inline vec3f_t Scalar(const vec3f_t &v, float s)
{
	vec3f_t r;
	r.x = v.x * s;
	r.y = v.y * s;
	r.z = v.z * s;

	return r;
}


inline vec4f_t Scalar(const vec4f_t &v, float s)
{
	vec4f_t r;
	r.x = v.x * s;
	r.y = v.y * s;
	r.z = v.z * s;
	r.w = v.w * s;

	return r;
}


inline float Dot(const vec3f_t& v, const vec3f_t& s)
{
	float r;
	r = (v.x * s.x) + (v.y * s.y) + (v.z * s.z);

	return r;
}


inline vec3f_t Cross(const vec3f_t& v, const vec3f_t& s)
{
	vec3f_t r;
	r.x = (v.y * s.z) - (v.z * s.y);
	r.y = (v.z * s.x) - (v.x * s.z);
	r.z = (v.x * s.y) - (v.y * s.x);

	return r;
}


inline vec3f_t Normalize(const vec3f_t &v)
{
	vec3f_t r;
	float len = sqrtf((v.x * v.x) + (v.y * v.y) + (v.z * v.z));
	r.x = v.x / len;
	r.y = v.y / len;
	r.z = v.z / len;

	return r;
}


inline void SetTransform(mat4f_t *t)
{
	memset(t->m, 0, sizeof(float) * 16);
	t->m[0][0] = 1;
	t->m[1][1] = 1;
	t->m[2][2] = 1;
	t->m[3][3] = 1;
}


inline void SetCameraView(mat4f_t* t, const vec3f_t& position, const vec3f_t& target, const vec3f_t& upAxis)
{
	SetTransform(t);

	vec3f_t direction = Normalize(position - target);
	vec3f_t right = Normalize(Cross(upAxis, direction));
	vec3f_t up = Normalize(Cross(direction, right));

	t->m[0][0] = right.x;
	t->m[0][1] = right.y;
	t->m[0][2] = right.z;

	t->m[1][0] = up.x;
	t->m[1][1] = up.y;
	t->m[1][2] = up.z;

	t->m[2][0] = direction.x;
	t->m[2][1] = direction.y;
	t->m[2][2] = direction.z;

	mat4f_t translate = {};
	SetTransform(&translate);
	translate.m[0][3] -= position.x;
	translate.m[1][3] -= position.y;
	translate.m[2][3] -= position.z;

	*t *= translate;
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


inline void MatrixRotate(mat4f_t* t, float d, const vec3f_t& r)
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


inline void Scale(mat4f_t *t, float s)
{
	t->m[0][0] *= s;
	t->m[1][1] *= s;
	t->m[2][2] *= s;
}


inline void Translate(mat4f_t *t, const vec3f_t &s)
{
	t->m[0][3] += s.x;
	t->m[1][3] += s.y;
	t->m[2][3] += s.z;
}


inline vec4f_t MultiplyV4_M4(const vec4f_t &v, const mat4f_t &m)
{
	vec4f_t res;

	res.x = (v.x * m.m[0][0]) + (v.x * m.m[0][1]) + (v.x * m.m[0][2]) + (v.x * m.m[0][3]);
	res.y = (v.y * m.m[1][0]) + (v.y * m.m[1][1]) + (v.y * m.m[1][2]) + (v.y * m.m[1][3]);
	res.z = (v.z * m.m[2][0]) + (v.z * m.m[2][1]) + (v.z * m.m[2][2]) + (v.z * m.m[2][3]);
	res.w = (v.w * m.m[3][0]) + (v.w * m.m[3][1]) + (v.w * m.m[3][2]) + (v.w * m.m[3][3]);

	return res;
}


inline mat4f_t InverseM4(const mat4f_t& m)
{
	float s0 = m.m[0][0] * m.m[1][1] - m.m[1][0] * m.m[0][1];
	float s1 = m.m[0][0] * m.m[1][2] - m.m[1][0] * m.m[0][2];
	float s2 = m.m[0][0] * m.m[1][3] - m.m[1][0] * m.m[0][3];
	float s3 = m.m[0][1] * m.m[1][2] - m.m[1][1] * m.m[0][2];
	float s4 = m.m[0][1] * m.m[1][3] - m.m[1][1] * m.m[0][3];
	float s5 = m.m[0][2] * m.m[1][3] - m.m[1][2] * m.m[0][3];

	float c5 = m.m[2][2] * m.m[3][3] - m.m[3][2] * m.m[2][3];
	float c4 = m.m[2][1] * m.m[3][3] - m.m[3][1] * m.m[2][3];
	float c3 = m.m[2][1] * m.m[3][2] - m.m[3][1] * m.m[2][2];
	float c2 = m.m[2][0] * m.m[3][3] - m.m[3][0] * m.m[2][3];
	float c1 = m.m[2][0] * m.m[3][2] - m.m[3][0] * m.m[2][2];
	float c0 = m.m[2][0] * m.m[3][1] - m.m[3][0] * m.m[2][1];

	float invdet = 1.0f / (s0 * c5 - s1 * c4 + s2 * c3 + s3 * c2 - s4 * c1 + s5 * c0);

	mat4f_t res;

	res.m[0][0] = (m.m[1][1] * c5 - m.m[1][2] * c4 + m.m[1][3] * c3) * invdet;
	res.m[0][1] = (-m.m[0][1] * c5 + m.m[0][2] * c4 - m.m[0][3] * c3) * invdet;
	res.m[0][2] = (m.m[3][1] * s5 - m.m[3][2] * s4 + m.m[3][3] * s3) * invdet;
	res.m[0][3] = (-m.m[2][1] * s5 + m.m[2][2] * s4 - m.m[2][3] * s3) * invdet;

	res.m[1][0] = (-m.m[1][0] * c5 + m.m[1][2] * c2 - m.m[1][3] * c1) * invdet;
	res.m[1][1] = (m.m[0][0] * c5 - m.m[0][2] * c2 + m.m[0][3] * c1) * invdet;
	res.m[1][2] = (-m.m[3][0] * s5 + m.m[3][2] * s2 - m.m[3][3] * s1) * invdet;
	res.m[1][3] = (m.m[2][0] * s5 - m.m[2][2] * s2 + m.m[2][3] * s1) * invdet;

	res.m[2][0] = (m.m[1][0] * c4 - m.m[1][1] * c2 + m.m[1][3] * c0) * invdet;
	res.m[2][1] = (-m.m[0][0] * c4 + m.m[0][1] * c2 - m.m[0][3] * c0) * invdet;
	res.m[2][2] = (m.m[3][0] * s4 - m.m[3][1] * s2 + m.m[3][3] * s0) * invdet;
	res.m[2][3] = (-m.m[2][0] * s4 + m.m[2][1] * s2 - m.m[2][3] * s0) * invdet;

	res.m[3][0] = (-m.m[1][0] * c3 + m.m[1][1] * c1 - m.m[1][2] * c0) * invdet;
	res.m[3][1] = (m.m[0][0] * c3 - m.m[0][1] * c1 + m.m[0][2] * c0) * invdet;
	res.m[3][2] = (-m.m[3][0] * s3 + m.m[3][1] * s1 - m.m[3][2] * s0) * invdet;
	res.m[3][3] = (m.m[2][0] * s3 - m.m[2][1] * s1 + m.m[2][2] * s0) * invdet;

	return res;
}

}


#endif
