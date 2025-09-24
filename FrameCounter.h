#pragma once

#include "Config.h"

class FrameCounter
{
public:
	double getFPS();
	void update();

	int currentFrame = 0;
	double FPS = -1;
};

