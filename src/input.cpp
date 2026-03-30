#include "input.h"

void input::axis3D::Refresh() {
	//TODO
}
	

//Callbacks for essential window functions
//For now Key Input processes "ESC" to close program and WASD for camera positioning
void KeyInputCallback(GLFWwindow* Window, int key, int scancode, int action, int mods) {

	
	window_handler_t* WinHND = (window_handler_t*)glfwGetWindowUserPointer(Window);

	// Check if the UI should be pulling focus
	if (WinHND->ImIO.WantCaptureKeyboard)
	{
		return;
	}
	

	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(Window, true);
	}

	//Camera positioning

	if (key == GLFW_KEY_W)
	{
		switch (action) {
			case GLFW_PRESS:
				WinHND->Camera.instructions.forward = true;
				break;
			case GLFW_RELEASE:
				WinHND->Camera.instructions.forward = false;
				break;
		}
	}
	if (key == GLFW_KEY_S)
	{
		switch (action) {
		case GLFW_PRESS:
			WinHND->Camera.instructions.backward = true;
			break;
		case GLFW_RELEASE:
			WinHND->Camera.instructions.backward = false;
			break;
		}
	}
	if (key == GLFW_KEY_A)
	{
		switch (action) {
		case GLFW_PRESS:
			WinHND->Camera.instructions.left = true;
			break;
		case GLFW_RELEASE:
			WinHND->Camera.instructions.left = false;
			break;
		}
	}
	if (key == GLFW_KEY_D)
	{
		switch (action) {
		case GLFW_PRESS:
			WinHND->Camera.instructions.right = true;
			break;
		case GLFW_RELEASE:
			WinHND->Camera.instructions.right = false;
			break;
		}
	}
	if (key == GLFW_KEY_SPACE)
	{
		switch (action) {
		case GLFW_PRESS:
			WinHND->Camera.instructions.up = true;
			break;
		case GLFW_RELEASE:
			WinHND->Camera.instructions.up = false;
			break;
		}
	}
	if (key == GLFW_KEY_LEFT_SHIFT)
	{
		switch (action) {
		case GLFW_PRESS:
			WinHND->Camera.instructions.down = true;
			break;
		case GLFW_RELEASE:
			WinHND->Camera.instructions.down = false;
			break;
		}
	}
}

//Mouse position callback will point the camera when right mouse is held down
void MousePosCallback(GLFWwindow* Window, double mx, double my)
{
	
	window_handler_t* WinHND = (window_handler_t*)glfwGetWindowUserPointer(Window);

	// Check if the UI should be pulling focus
	if (WinHND->ImIO.WantCaptureMouse)
	{
		return;
	}
	

	if (WinHND->FirstCameraMove)
	{
		WinHND->PrevMouseX = mx;
		WinHND->PrevMouseY = my;
		WinHND->FirstCameraMove = 0;
	}

	float xoffset = mx - WinHND->PrevMouseX;
	float yoffset = WinHND->PrevMouseY - my;

	WinHND->PrevMouseX = mx;
	WinHND->PrevMouseY = my;

	
	if (glfwGetMouseButton(Window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
	{
		glfwSetInputMode(Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		WinHND->Camera.LookAtMouse(xoffset, yoffset);
	}

	if (glfwGetMouseButton(Window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_RELEASE)
	{
		glfwSetInputMode(Window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}
	
}