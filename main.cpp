#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

#include "Shaders/Shader.h"

GLFWwindow* window;
bool isToggleFirewrameKeyDown = false;
GLenum currentPolygonMode = GL_FILL;
glm::vec2 pos(0,0);

const GLenum TOGGLE_POLYGON_KEY = GLFW_KEY_Q;
const int WINDOW_WIDTH = 1024;
const int WINDOW_HEIGHT = 1024;
const float MOVE_SPEED = 0.01f;

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

bool isKeyPressed(int key)
{
    return (glfwGetKey(window, key) == GLFW_PRESS);
}

void toggleWireframe(bool _isEnabled)
{
    GLenum targetPolygonMode = _isEnabled ? GL_LINE : GL_FILL;

    if (currentPolygonMode != targetPolygonMode)
    {
        glPolygonMode(GL_FRONT_AND_BACK, targetPolygonMode);
        std::cout << "Wireframe Mode: " << _isEnabled << "\n";
        currentPolygonMode = targetPolygonMode;
    }
}

void processInput(GLFWwindow* window)
{

    /*ESCAPE*/

    if (isKeyPressed(GLFW_KEY_ESCAPE))
    {
        glfwSetWindowShouldClose(window, true);
    }

    /*TOGGLE WIREFRAME*/

    if (isKeyPressed(GLFW_KEY_Q) && !isToggleFirewrameKeyDown)
    {
        isToggleFirewrameKeyDown = true;
        toggleWireframe(currentPolygonMode != GL_LINE);
    }
    else if (!isKeyPressed(GLFW_KEY_Q) && isToggleFirewrameKeyDown)
    {
        isToggleFirewrameKeyDown = false;
    }

    /*MOVEMENT*/

    if (isKeyPressed(GLFW_KEY_A))
    {
        pos.x -= MOVE_SPEED;
    }
    if (isKeyPressed(GLFW_KEY_D))
    {
        pos.x += MOVE_SPEED;
    }
    if (isKeyPressed(GLFW_KEY_W))
    {
        pos.y += MOVE_SPEED;
    }
    if (isKeyPressed(GLFW_KEY_S))
    {
        pos.y -= MOVE_SPEED;
    }
}


int main(void)
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

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    /*Initialize GLAD*/

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to Initialize the GLAD";
    }

    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

    /*Create shader*/

    Shader myShader;
    myShader.attachShader("./Shaders/shadervs.glsl", GL_VERTEX_SHADER);
    myShader.attachShader("./Shaders/shaderfs.glsl", GL_FRAGMENT_SHADER);
    myShader.link();

    /*CUBE*/

    const float LENGTH = 0.1;

    float vertices[] = {
        -LENGTH/2, LENGTH/2, 0,
        -LENGTH/2, -LENGTH/2, 0,
        LENGTH/2, -LENGTH/2, 0,

        -LENGTH/2, LENGTH/2, 0,
        LENGTH/2, LENGTH/2, 0,
        LENGTH/2, -LENGTH/2, 0
    };

    /*VAO & VBO*/

    unsigned int VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    unsigned int VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float)*3, (void*)0);
    glEnableVertexAttribArray(0);
    
    /*Position Attribute*/

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    int posLocation = myShader.getUniformLocation("uPos");

    /*Window loop*/

    while (!glfwWindowShouldClose(window))
    {

        /*Clear Window*/
        glClearColor(0.2f, 0.3f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);       

        glUniform2f(posLocation, pos.x, pos.y);
        myShader.use();
        glBindVertexArray(VAO);
        glEnableVertexAttribArray(0);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        glfwSwapBuffers(window);
        glfwPollEvents();
        processInput(window);
    }

    glfwTerminate();
    return 0;
}