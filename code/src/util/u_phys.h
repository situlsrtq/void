#ifndef MBOX_PHYSICS_H
#define MBOX_PHYSICS_H


#include "u_math.h"
#include "../window.h"


namespace uPHYS
{


struct ray_info_t
{
	uMATH::vec3f_t Direction;
	uMATH::vec3f_t Start;
};


inline uMATH::vec3f_t CastWorldRay(float MouseX, float MouseY, const window_handler_t& Window)
{
	uMATH::vec4f_t ray4f;

	float xvp = (2.0f * MouseX / (float)Window.Width) - 1.0f;
	float yvp = (2.0f * MouseY / (float)Window.Height) - 1.0f;
	float d = 1.0f / tan(45.0f * RADIAN);
	float ar = (float)Window.Height / (float)Window.Width;
	float xview = xvp / d;
	float yview = (yvp / d) * ar;
	float z = -1.0f;

	ray4f.x = xview;			// Ray Termination in NDC
	ray4f.y = yview;
	ray4f.z = z;
	ray4f.w = 1.0f;

	uMATH::mat4f_t WorldSpace = uMATH::InverseM4(Window.View);
	ray4f = uMATH::MultiplyV4_M4(ray4f, WorldSpace);
	ray4f = Scalar(ray4f, (1.0f / ray4f.w));

	uMATH::vec3f_t out = { ray4f.x, ray4f.y, ray4f.z };
	uMATH::vec3f_t src = Window.Camera.Position;
	out = out - src;
	out = uMATH::Normalize(out);

	return out;
}


inline bool CheckRayOBBCollision(uMATH::vec3f_t Origin, uMATH::vec3f_t Direction, uMATH::vec3f_t MinAABB, uMATH::vec3f_t MaxAABB, uMATH::mat4f_t Model, float* Distance)
{
	float tmin = -100000.0f;
	float tmax = 100000.0f;

	uMATH::vec3f_t OBBPosition = { Model.m[0][3], Model.m[1][3], Model.m[2][3] };
	uMATH::vec3f_t delta = OBBPosition - Origin;

	uMATH::vec3f_t axis = { Model.m[0][0], Model.m[1][0], Model.m[2][0] };			// X Axis
	float e = uMATH::Dot(axis, delta);
	float f = 1 / uMATH::Dot(Direction, axis);

	if (fabs(f) > 0.001f)
	{
		float t1 = (e + MinAABB.x) * f;
		float t2 = (e + MaxAABB.x) * f;

		if (t1 > t2)
		{
			float temp = t1;
			t1 = t2;
			t2 = temp;
		}

		if (t1 > tmin)
		{
			tmin = t1;
		}
		if (t2 < tmax)
		{
			tmax = t2;
		}
		if (tmin > tmax || tmax < 0)
		{
			*Distance = 0.0f;
			return false;
		}

	}
	else if (-e + MinAABB.x > 0.0f || -e + MaxAABB.x < 0.0f)
	{
		*Distance = 0.0f;
		return false;
	}

	axis = { Model.m[0][1], Model.m[1][1], Model.m[2][1] };			// Y Axis
	e = uMATH::Dot(axis, delta);
	f = 1 / uMATH::Dot(Direction, axis);

	if (fabs(f) > 0.001f)
	{
		float t1 = (e + MinAABB.y) * f;
		float t2 = (e + MaxAABB.y) * f;

		if (t1 > t2)
		{
			float temp = t1;
			t1 = t2;
			t2 = temp;
		}

		if (t1 > tmin)
		{
			tmin = t1;
		}
		if (t2 < tmax)
		{
			tmax = t2;
		}
		if (tmin > tmax || tmax < 0)
		{
			*Distance = 0.0f;
			return false;
		}

	}
	else if (-e + MinAABB.y > 0.0f || -e + MaxAABB.y < 0.0f)
	{
		*Distance = 0.0f;
		return false;
	}

	axis = { Model.m[0][2], Model.m[1][2], Model.m[2][2] };			// Z Axis
	e = uMATH::Dot(axis, delta);
	f = 1 / uMATH::Dot(Direction, axis);

	if (fabs(f) > 0.001f)
	{
		float t1 = (e + MinAABB.z) * f;
		float t2 = (e + MaxAABB.z) * f;

		if (t1 > t2)
		{
			float temp = t1;
			t1 = t2;
			t2 = temp;
		}

		if (t1 > tmin)
		{
			tmin = t1;
		}
		if (t2 < tmax)
		{
			tmax = t2;
		}
		if (tmin > tmax || tmax < 0)
		{
			*Distance = 0.0f;
			return false;
		}

	}
	else if (-e + MinAABB.z > 0.0f || -e + MaxAABB.z < 0.0f)
	{
		*Distance = 0.0f;
		return false;
	}

	if (tmin > 0)
	{
		*Distance = tmin;
		return tmin;
	}
	else
	{
		*Distance = tmax;
		return true;
	}
}


}

#endif
