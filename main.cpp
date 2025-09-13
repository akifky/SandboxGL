#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <vector>
#include <string>
#include <random>

#include "Shaders/Shader.h"
#include "Objects/SandboxGUI.h"



const GLenum TOGGLE_POLYGON_KEY = GLFW_KEY_Q;
const int FPS_LIMIT = 60;
const int WINDOW_WIDTH = 920;
const int WINDOW_HEIGHT = 920;
const int SIMULATION_GRID_WIDTH = 460;
const int SIMULATION_GRID_HEIGHT = 460;
const int SIMULATION_INTERVAL_IN_FRAMES = 3;
int BRUSH_SIZE = 30;
float BRUSH_DENSITY = 0.01f; // Between 0 and 1

float cellSize = 2.0f / std::max(SIMULATION_GRID_WIDTH, SIMULATION_GRID_HEIGHT);

std::default_random_engine rng(std::random_device{}());
std::uniform_real_distribution<float> dist(0.0f, 1.0f);
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
SandboxGUI* sandboxGui;
double FPS = -1;

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
    return (frameCounter % SIMULATION_INTERVAL_IN_FRAMES == 0);
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
    }
    if (isKeyPressed(GLFW_KEY_2))
    {
        selectedType = TILE_WATER;
    }
}

void updateFpsValue(GLFWwindow* window) {
    static double previousTime = 0.0;
    static int frameCount = 0;
    double currentTime = glfwGetTime();
    frameCount++;
    // Update FPS every second
    if (currentTime - previousTime >= 1.0) {
        FPS = double(frameCount) / (currentTime - previousTime);
        frameCount = 0;
        previousTime = currentTime;
    }
}

std::string getTileName(TileType type)
{
    switch (type)
    {
    case TILE_EMPTY: return "Air";
    case TILE_SAND: return "Sand";
    case TILE_WATER: return "Water";

    default: return "Unknown";
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

        

        for (int dy = BRUSH_SIZE / 2 * -1; dy <= BRUSH_SIZE/ 2; ++dy) {
            for (int dx = BRUSH_SIZE / 2 * -1; dx <= BRUSH_SIZE / 2; ++dx) {

                if (dist(rng) < BRUSH_DENSITY)
                {
                    if (isValidTile(gridX + dx, gridY + dy))
                    {
                        grid[gridY + dy][gridX + dx] = selectedType;
                    }
                }
            }
        }       
    }
}


void swapTiles(int x1, int y1, int x2, int y2)
{
    if (!isValidTile(x1, y1) || !isValidTile(x2, y2)) { return; }

    TileType cell_1 = nextGrid[y1][x1];
    TileType cell_2 = nextGrid[y2][x2];
    nextGrid[y1][x1] = cell_2;
    nextGrid[y2][x2] = cell_1;
}

bool moveTile(int tileX, int tileY, int moveX, int moveY)
{
    int newX = tileX + moveX;
    int newY = tileY + moveY;
    if (!isValidTile(newX, newY) || !isValidTile(tileX, tileY)) { return false; } // Tile is not valid

    TileType tile = grid[tileY][tileX];
    TileType targetTile = nextGrid[newY][newX];

    if (tile == TILE_EMPTY) { return false; } // Source tile is empty

    switch (tile)
    {
    case TILE_SAND: // Sand Movement
        switch (targetTile)
        {

        case TILE_EMPTY: // SAND X AIR
            swapTiles(tileX, tileY, newX, newY);
            return true;
        case TILE_WATER: // SAND X WATER
            swapTiles(tileX, tileY, newX, newY);
            return true;

        default: return false;
        }
    case TILE_WATER:
        switch (targetTile)
        {
        case TILE_EMPTY:
            swapTiles(tileX,tileY,newX,newY);
            return true;
        default: return false;
        }

    default: return false;
    }

}

void simulateGrid()
{
    static double previousTime = 0.0;
    double currentTime = glfwGetTime();
    if (isSimulationFrame())
    {
        previousTime = currentTime;

        // Copy the grid
        for (int y = 0; y < SIMULATION_GRID_HEIGHT; ++y) {
            for (int x = 0; x < SIMULATION_GRID_WIDTH; ++x) {
                nextGrid[y][x] = grid[y][x];
            }
        }

        // Bottom Left - > Top Right loop
        for (int y = SIMULATION_GRID_HEIGHT - 1; y >= 0; --y) {
            for (int x = 0; x < SIMULATION_GRID_WIDTH; ++x) {

                TileType currentTile = grid[y][x];

                switch (grid[y][x]) {

                case TILE_SAND:
                    if (moveTile(x, y, 0, 1)) { break; } // Down
                    else if (moveTile(x, y, -1, 1)) { break; } // Down - Left
                    else if (moveTile(x, y, 1, 1)) { break; } // Down - Right
                    break;

                case TILE_WATER:
                    if (moveTile(x, y, 0, 1)) { break; } // Down
                    else if (moveTile(x, y, -1, 1)) { break; } // Down - Left
                    else if (moveTile(x, y, 1, 1)) { break; } // Down - Right

                    else if (moveTile(x, y, -4, 0)) { break; } // Left
                    else if (moveTile(x, y, 4, 0)) { break; } // Right
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
    sandboxGui = new SandboxGUI(window);

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

        sandboxGui->addText("FPS: " + std::to_string(int(FPS)));
        sandboxGui->addText("Instance Count: " + std::to_string(int(instanceCount)));
        sandboxGui->addText("Type: " + getTileName(selectedType));
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