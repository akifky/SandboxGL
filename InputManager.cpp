#include "InputManager.h"
#include "Config.h"
#include <iostream>
#include <random>

GLenum currentPolygonMode = GL_FILL;
std::default_random_engine rng(std::random_device{}());
std::uniform_real_distribution<float> dist(0.0f, 1.0f);

InputManager::InputManager(GLFWwindow* window, Simulation* sim)
{
    this->_window = window;
    this->_sim = sim;
    glfwSetWindowUserPointer(window, this);
	glfwSetKeyCallback(window, [](GLFWwindow* w, int key, int scancode, int action, int mods) {
        static_cast<InputManager*>(glfwGetWindowUserPointer(w))->key_callback(w, key, scancode, action, mods);
		});
}

bool InputManager::isKeyPressed(int key)
{
    return (glfwGetKey(_window, key) == GLFW_PRESS);
}

bool InputManager::isMousePressed(int key)
{
    if (_window == nullptr) { return false; }
    return (glfwGetMouseButton(_window, key) == GLFW_PRESS);
}

void InputManager::toggleWireframe(bool _isEnabled)
{
    GLenum targetPolygonMode = _isEnabled ? GL_LINE : GL_FILL;

    if (currentPolygonMode != targetPolygonMode)
    {
        glPolygonMode(GL_FRONT_AND_BACK, targetPolygonMode);
        currentPolygonMode = targetPolygonMode;
    }
}

void InputManager::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    /*TOGGLE WIREFRAME*/

    if (key == TOGGLE_POLYGON_KEY)
    {
        if (action == GLFW_PRESS)
        {
            toggleWireframe(currentPolygonMode == GL_FILL);
        }
    }

    /*ESCAPE*/

    if (isKeyPressed(GLFW_KEY_ESCAPE))
    {
        glfwSetWindowShouldClose(window, true);
    }

    if (isKeyPressed(GLFW_KEY_1))
    {
        selectedType = Simulation::TILE_SAND;
    }
    if (isKeyPressed(GLFW_KEY_2))
    {
        selectedType = Simulation::TILE_WATER;
    }
}

void InputManager::processInput(GLFWwindow* window)
{
    if (isMousePressed(GLFW_MOUSE_BUTTON_1))
    {
        double cursorX, cursorY;
        glfwGetCursorPos(window, &cursorX, &cursorY);

        float cellPixelSizeX = (float)WINDOW_WIDTH / (float)SIMULATION_GRID_WIDTH;
        float cellPixelSizeY = (float)WINDOW_HEIGHT / (float)SIMULATION_GRID_HEIGHT;

        int gridX = (int)(cursorX / cellPixelSizeX);
        int gridY = (int)(cursorY / cellPixelSizeY);


        for (int dy = BRUSH_SIZE / 2 * -1; dy <= BRUSH_SIZE / 2; ++dy) {
            for (int dx = BRUSH_SIZE / 2 * -1; dx <= BRUSH_SIZE / 2; ++dx) {

                if (dist(rng) < BRUSH_DENSITY)
                {
                    if (_sim->isValidTile(gridX + dx, gridY + dy))
                    {
                        _sim->setTile(gridX + dx, gridY + dy, selectedType);

                    }
                }
            }
        }
    }
}