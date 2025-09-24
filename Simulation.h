#pragma once

#include <vector>
#include <glm/glm.hpp>
#include "FrameCounter.h"
#include <string>

class Simulation
{
public:
	enum TileType {
		TILE_EMPTY = 0,
		TILE_SAND = 1,
		TILE_WATER = 2
	};

	void update();
	void calculateInstanceData();
	bool isSimulationFrame(FrameCounter* frameCounter);
	bool isValidTile(int x, int y);
	bool moveTile(int tileX, int tileY, int moveX, int moveY);
	void swapTiles(int x1, int y1, int x2, int y2);
	std::string getTileName(TileType type);
	int getInstanceCount() { return instanceCount; }
	float getCellSize() { return cellSize; }
	std::vector<glm::vec2> getCellPositions() { return cellPositions; }
	std::vector<TileType> getCellTypes() { return cellTypes; }
	void setTile(int x, int y, TileType type);
	void setNextTile(int x, int y, TileType type);
	double getFPS();

private:
	TileType grid[SIMULATION_GRID_HEIGHT][SIMULATION_GRID_WIDTH] = { TILE_EMPTY };
	TileType nextGrid[SIMULATION_GRID_HEIGHT][SIMULATION_GRID_WIDTH] = { TILE_EMPTY };
	float cellSize = 2.0f / std::max(SIMULATION_GRID_WIDTH, SIMULATION_GRID_HEIGHT);
	std::vector<glm::vec2> cellPositions;
	std::vector<TileType> cellTypes;
	int instanceCount = 0;

	void simulateGrid();
};

