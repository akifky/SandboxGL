#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <vector>
#include <string>

#include "Shaders/Shader.h"
#include "Objects/SandboxGUI.h"
#include "Config.h"
#include "InputManager.h"
#include "Simulation.h"

GLFWwindow* window;
Simulation* sim = new Simulation();

unsigned int quadVBO, instancePositionVBO, tileTypeVBO;

float unitQuad[] = {
    -0.5f,  0.5f, 0.0f,  // Top Left
	-0.5f, -0.5f, 0.0f,  // Bottom Left 
     0.5f, -0.5f, 0.0f,  // Bottom Right

    -0.5f,  0.5f, 0.0f,  // Top Left
     0.5f,  0.5f, 0.0f,  // Top Right
	 0.5f, -0.5f, 0.0f   // Bottom Right
};
Shader* myShader = NULL;
SandboxGUI* sandboxGui;

void sendDataToGPU() {
    glBindBuffer(GL_ARRAY_BUFFER, instancePositionVBO);
    glBufferData(GL_ARRAY_BUFFER, sim->getCellPositions().size() * sizeof(glm::vec2),
        sim->getCellPositions().data(), GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, tileTypeVBO);
    glBufferData(GL_ARRAY_BUFFER, sim->getCellTypes().size() * sizeof(Simulation::TileType),
        sim->getCellTypes().data(), GL_DYNAMIC_DRAW);
}

int main()
{
    /*Initialize GLFW*/

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


    /*Create window*/

    window = glfwCreateWindow(WINDOW_WIDTH,WINDOW_HEIGHT,"Sandbox",NULL,NULL);
    if (!window) {
        std::cout << "Failed to create the window";
        glfwTerminate();
        return -1;
    }
    InputManager* inputManager = new InputManager(window, sim);

    // Callback Events
    glfwMakeContextCurrent(window);

    // Initialize GLAD

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to Initialize the GLAD";
    }

    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

    // Create Shader
	myShader = new Shader();
    myShader->attachShader("./Shaders/shadervs.glsl", GL_VERTEX_SHADER);
    myShader->attachShader("./Shaders/shaderfs.glsl", GL_FRAGMENT_SHADER);
    myShader->link();

    //Create GUI
    sandboxGui = new SandboxGUI(window);

    // VAO & VBO

    unsigned int VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &instancePositionVBO);
	glGenBuffers(1, &tileTypeVBO);
	glGenBuffers(1, &quadVBO);

	sim->calculateInstanceData();

    // quadVBO
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(unitQuad), unitQuad, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribDivisor(0, 0);

    // instancePositionVBO
    glBindBuffer(GL_ARRAY_BUFFER, instancePositionVBO);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribDivisor(1, 1);

    // tileTypeVBO
    glBindBuffer(GL_ARRAY_BUFFER, tileTypeVBO);
    glVertexAttribIPointer(2, 1, GL_INT, sizeof(Simulation::TileType), (void*)0);
    glEnableVertexAttribArray(2);
    glVertexAttribDivisor(2, 1);

    glBindVertexArray(VAO);

    // Use Shader
    int cellSizeLocation = myShader->getUniformLocation("uCellSize");
    myShader->use();

    glUniform1f(cellSizeLocation, sim->getCellSize());

    glfwSwapInterval(1);

    /*Window loop*/

    while (!glfwWindowShouldClose(window))
    {

        
        if (!(sandboxGui->io->WantCaptureMouse && sandboxGui->io->MouseDown)) { inputManager->processInput(window); }

        sim->update();
        sandboxGui->update();
        sendDataToGPU();

        /*Clear Window*/
        glClearColor(0.2f, 0.3f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);       
        glDrawArraysInstanced(GL_TRIANGLES, 0, 6, sim->getInstanceCount());

        sandboxGui->addText("FPS: " + std::to_string(int(sim->getFPS())));
        sandboxGui->addText("Instance Count: " + std::to_string(int(sim->getInstanceCount())));
        sandboxGui->addText("Type: " + sim->getTileName(inputManager->selectedType));
        sandboxGui->addIntSlider("Brush Size", BRUSH_SIZE, 1, 50);
        sandboxGui->addFloatSlider("Brush Density", BRUSH_DENSITY, 0.005f, 0.05f);
		sandboxGui->render();

        glfwSwapBuffers(window);
        glfwPollEvents();
           
    }

    sandboxGui->destroy();
    glfwTerminate();
    return 0;
}