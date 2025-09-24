#include "FrameCounter.h"

void FrameCounter::update() {
	static double previousSeconds = 0.0;
	double currentSeconds = glfwGetTime(); // Returns number of seconds since GLFW started, as a double float
	double elapsedSeconds = currentSeconds - previousSeconds;
	
	if (elapsedSeconds > 1.0)
	{
		previousSeconds = currentSeconds;
		FPS = (double)currentFrame / elapsedSeconds;
		currentFrame = 0;
	}
	
	currentFrame++;
}

double FrameCounter::getFPS() {
	return FPS;
}