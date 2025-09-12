#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

#include "Shaders/Shader.h"
#include "Objects/SandboxWindow.h"
#include <vector>
#include <string>

const GLenum TOGGLE_POLYGON_KEY = GLFW_KEY_Q;
const int FPS_LIMIT = 60;
const int WINDOW_WIDTH = 920;
const int WINDOW_HEIGHT = 920;
const int SIMULATION_GRID_WIDTH = 460;
const int SIMULATION_GRID_HEIGHT = 460;
const int SIMULATION_EVERY_N_FRAMES = 3;

float cellSize = 2.0f / std::max(SIMULATION_GRID_WIDTH, SIMULATION_GRID_HEIGHT);

GLFWwindow* window;
GLenum currentPolygonMode = GL_FILL;
unsigned int quadVBO, instancePositionVBO, tileTypeVBO;
enum TileType {
    TILE_EMPTY = 0,
    TILE_SAND = 1,
    TILE_WATER = 2,
};
float unitQuad[] = {
    -0.5f,  0.5f, 0.0f,  // Top Left
	-0.5f, -0.5f, 0.0f,  // Bottom Left 
     0.5f, -0.5f, 0.0f,  // Bottom Right

    -0.5f,  0.5f, 0.0f,  // Top Left
     0.5f,  0.5f, 0.0f,  // Top Right
	 0.5f, -0.5f, 0.0f   // Bottom Right
};
Shader* myShader = NULL;
std::vector<glm::vec2> cellPositions;
std::vector<TileType> cellTypes;
int instanceCount = 0;
int frameCounter = 0;
TileType grid[SIMULATION_GRID_HEIGHT][SIMULATION_GRID_WIDTH];
TileType nextGrid[SIMULATION_GRID_HEIGHT][SIMULATION_GRID_WIDTH]; // For double buffering
TileType selectedType = TILE_SAND;
SandboxWindow* sandboxGui;
std::string FPSstring;
std::string TileString;

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

bool isKeyPressed(int key)
{
    return (glfwGetKey(window, key) == GLFW_PRESS);
}

bool isMousePressed(int key)
{
    return (glfwGetMouseButton(window, key) == GLFW_PRESS);
}

bool isSimulationFrame()
{
    return (frameCounter % SIMULATION_EVERY_N_FRAMES == 0);
}

bool isValidTile(int x, int y)
{
    return (x >= 0 && x < SIMULATION_GRID_WIDTH && y >= 0 && y < SIMULATION_GRID_HEIGHT);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
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
        selectedType = TILE_SAND;
        TileString = "Sand";
    }
    if (isKeyPressed(GLFW_KEY_2))
    {
        selectedType = TILE_WATER;
        TileString = "Water";
    }
}

void updateFpsValue(GLFWwindow* window) {
    static double previousTime = 0.0;
    static int frameCount = 0;
    double currentTime = glfwGetTime();
    frameCount++;
    // Update FPS every second
    if (currentTime - previousTime >= 1.0) {
        double fps = double(frameCount) / (currentTime - previousTime);
        FPSstring = "FPS: " + std::to_string(int(fps));
        frameCount = 0;
        previousTime = currentTime;
    }
}

void setSelectedTileType(TileType type)
{
    switch (type) {

    case TILE_EMPTY:
        TileString = "Empty";
        break;
    case TILE_SAND:
        TileString = "Sand";
        break;
    case TILE_WATER:
        TileString = "Water";
        break;
    
    }
}

void processInput(GLFWwindow* window)
{
    if (isMousePressed(GLFW_MOUSE_BUTTON_1) && !(sandboxGui->io->WantCaptureMouse && sandboxGui->io->MouseDown))
    {
        double cursorX, cursorY;
        glfwGetCursorPos(window, &cursorX, &cursorY);

        float cellPixelSizeX = (float)WINDOW_WIDTH / (float)SIMULATION_GRID_WIDTH;
        float cellPixelSizeY = (float)WINDOW_HEIGHT / (float)SIMULATION_GRID_HEIGHT;

        int gridX = (int)(cursorX / cellPixelSizeX);
        int gridY = (int)(cursorY / cellPixelSizeY);

        
        int brushSize = 10;
        for (int dy = brushSize / 2 * -1; dy <= brushSize / 2; dy++) {
            for (int dx = brushSize / 2 * -1; dx <= brushSize / 2; dx++) {

                if (isValidTile(gridX + dx, gridY + dy))
                {
                        grid[gridY + dy][gridX + dx] = selectedType;
                }
            }
        }       
    }
}

bool moveCell(int tileX, int tileY, int moveX, int moveY)
{
	int newX = tileX + moveX;
	int newY = tileY + moveY;

    TileType cell = grid[tileY][tileX];
    if (cell != TILE_EMPTY)
    {
        if (isValidTile(newX, newY) && grid[newY][newX] == TILE_EMPTY && nextGrid[newY][newX] == TILE_EMPTY)
        {
            nextGrid[newY][newX] = cell; // Move down
            nextGrid[tileY][tileX] = TILE_EMPTY; // Clear old position
            return true;
        }
    }
    
	return false; 

}

void simulateGrid()
{

	static double previousTime = 0.0;
	double currentTime = glfwGetTime();
    if (isSimulationFrame())
    {
		previousTime = currentTime;

        // Initialize nextGrid to current grid state
        for (int y = 0; y < SIMULATION_GRID_HEIGHT; ++y) {
            for (int x = 0; x < SIMULATION_GRID_WIDTH; ++x) {
                nextGrid[y][x] = grid[y][x];
            }
        }

        for (int y = 0; y < SIMULATION_GRID_HEIGHT; ++y) {
            for (int x = 0; x < SIMULATION_GRID_WIDTH; ++x) {

                TileType currentTile = grid[y][x];

                switch (grid[y][x]) {

                case TILE_SAND:
                    if (moveCell(x, y, 0, 1)) { break; } // Down
                    else if (moveCell(x, y, -1, 1)) { break; } // Down - Left
                    else if (moveCell(x, y, 1, 1)) { break; } // Down - Right
                    break;

                case TILE_WATER:
                    if (moveCell(x, y, 0, 1)) { break; } // Down
                    else if (moveCell(x, y, -1, 1)) { break; } // Down - Left
                    else if (moveCell(x, y, 1, 1)) { break; } // Down - Right

                    else if (moveCell(x, y, -4, 0)) { break; } // Left
                    else if (moveCell(x, y, 4, 0)) { break; } // Right
                    break;
                }
            }
        }
		// Swap grids
        std::swap(grid, nextGrid);
    }
}

void updateInstanceData() {

    // Clear previous data
    cellPositions.clear();
    cellTypes.clear();

    // Populate instance data based on the grid
    for (int y = 0; y < SIMULATION_GRID_HEIGHT; ++y) {
        for (int x = 0; x < SIMULATION_GRID_WIDTH; ++x) {
            if (grid[y][x] != TILE_EMPTY) {
                float worldX = (x * cellSize) - 1.0f + (cellSize / 2.0f);
                float worldY = 1.0f - (y * cellSize) - (cellSize / 2.0f);

                cellPositions.push_back(glm::vec2(worldX, worldY));
                cellTypes.push_back(grid[y][x]);
            }
        }
    }

    instanceCount = cellPositions.size();
}

void sendDataToGPU() {
    glBindBuffer(GL_ARRAY_BUFFER, instancePositionVBO);
    glBufferData(GL_ARRAY_BUFFER, cellPositions.size() * sizeof(glm::vec2),
        cellPositions.data(), GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, tileTypeVBO);
    glBufferData(GL_ARRAY_BUFFER, cellTypes.size() * sizeof(TileType),
        cellTypes.data(), GL_DYNAMIC_DRAW);
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

    // Callback Events
    glfwMakeContextCurrent(window);
	glfwSetKeyCallback(window, key_callback);

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
    sandboxGui = new SandboxWindow(window);

    // VAO & VBO

    unsigned int VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &instancePositionVBO);
	glGenBuffers(1, &tileTypeVBO);
	glGenBuffers(1, &quadVBO);

	updateInstanceData();

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
    glVertexAttribIPointer(2, 1, GL_INT, sizeof(TileType), (void*)0);
    glEnableVertexAttribArray(2);
    glVertexAttribDivisor(2, 1);

    glBindVertexArray(VAO);

    // Use Shader
    int cellSizeLocation = myShader->getUniformLocation("uCellSize");
    myShader->use();

    glUniform1f(cellSizeLocation, cellSize);

    glfwSwapInterval(1);

    setSelectedTileType(selectedType); //Update GUI Text

    /*Window loop*/

    while (!glfwWindowShouldClose(window))
    {
        frameCounter++;

        sandboxGui->update();
		updateFpsValue(window);
        processInput(window);

        simulateGrid();
        updateInstanceData();
        sendDataToGPU();

        /*Clear Window*/
        glClearColor(0.2f, 0.3f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);       
        glDrawArraysInstanced(GL_TRIANGLES, 0, 6, instanceCount);

        sandboxGui->addText(&FPSstring);
        sandboxGui->addText(&TileString);

        sandboxGui->render();

        glfwSwapBuffers(window);
        glfwPollEvents();
           
    }

    sandboxGui->destroy();
    glfwTerminate();
    return 0;
}