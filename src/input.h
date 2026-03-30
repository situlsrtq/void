#ifndef INPUT_H
#define INPUT_H

#include<glad/glad.h>
#include <GLFW/glfw3.h>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

#include "window.h"

/*
* 
*	Creating a name space 'input' for other modules to implement input as needed.
*	This is a start to separating input functionality from GLFW, in case we do write our own windowing later?
*	If we do drop GLFW, we would just need to replace how the 'input' namespace finds its raw data / values
*	Functionality should still remain fine everywhere else the namespace is used. 
*	Additionally, an input namespace will be helpful in developing with custom devices / controllers (DIY, MicroControllers)
*	THis is something I might want to do in the future. 
*	At the moment though, the input data GLFW exposes is fine for demoing how it works
*
*/

namespace input {

	struct input_type {
		virtual void Refresh() {};
		virtual ~input_type() {};
	};
	
	//Example:

	//input::axis3D : a wrapper of a glm vec3 that refreshes it values with input 
	struct axis3D : input_type {
		glm::vec3 values;

		//Must implement the refresh function, which means telling this how to update its values
		void Refresh();
	};


}

//Fallback methods

void KeyInputCallback(GLFWwindow* Window, int key, int scancode, int action, int mods);

void MousePosCallback(GLFWwindow* Window, double mx, double my);


#endif
