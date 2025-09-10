#pragma once

#include <glm/glm.hpp>

class Block {

public:
	Block(float x, float y, float size);

	glm::vec4 getColor() {
		return _color;
	}

	glm::vec3 getPosition() {
		return _position;
	}

	void move(glm::vec2 direction, float speed);

private:
	glm::vec4 _color;
	glm::vec3 _position;
};