#pragma once

#include <GLFW/glfw3.h>
#include "Simulation.h"

class InputManager
{
	public:
	Simulation::TileType selectedType = Simulation::TILE_SAND;

	InputManager(GLFWwindow* window, Simulation* sim);
	bool isKeyPressed(int key);
	bool isMousePressed(int key);
	void toggleWireframe(bool _isEnabled);
	void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
	void processInput(GLFWwindow* window);

	private:
	GLFWwindow* _window;
	Simulation* _sim;
};

