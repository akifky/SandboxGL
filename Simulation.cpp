#include <iostream>
#include "Simulation.h"
#include "FrameCounter.h"

FrameCounter* frameCounter = new FrameCounter();

void Simulation::update(){
    frameCounter->update();
    simulateGrid();
    calculateInstanceData();
}

std::string Simulation::getTileName(TileType type)
{
    switch (type)
    {
    case TILE_EMPTY: return "Air";
    case TILE_SAND: return "Sand";
    case TILE_WATER: return "Water";

    default: return "Unknown";
    }
}

double Simulation::getFPS()
{
    if (frameCounter == nullptr) { return -1; }
    return frameCounter->getFPS();
}

bool Simulation::isSimulationFrame(FrameCounter* frameCounter){
	return (frameCounter->currentFrame % SIMULATION_INTERVAL_IN_FRAMES == 0);
}

bool Simulation::isValidTile(int x, int y){
	return (x >= 0 && x < SIMULATION_GRID_WIDTH && y >= 0 && y < SIMULATION_GRID_HEIGHT);
}

void Simulation::setTile(int x, int y, TileType type){
    if (isValidTile(x, y)) {
        grid[y][x] = type;
    }
}

void Simulation::setNextTile(int x, int y, TileType type) {
    if (isValidTile(x, y)) {
        nextGrid[y][x] = type;
    }
}

void Simulation::swapTiles(int x1, int y1, int x2, int y2){
	TileType temp = nextGrid[y1][x1];
	nextGrid[y1][x1] = nextGrid[y2][x2];
	nextGrid[y2][x2] = temp;
}

bool Simulation::moveTile(int tileX, int tileY, int moveX, int moveY)
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
            swapTiles(tileX, tileY, newX, newY);
            return true;
        default: return false;
        }

    default: return false;
    }

}

void Simulation::calculateInstanceData() {

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

void Simulation::simulateGrid()
{
    if (isSimulationFrame(frameCounter))
    {
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