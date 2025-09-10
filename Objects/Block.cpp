#include "Block.h"
#include <cstdlib>
#include <time.h>

Block::Block(float x, float y, float size) {

	_position = glm::vec3(x, y, 0.0f);

	/*Random Color*/
	srand(time(NULL));
	_color.x = (std::rand() % 25) / 25.0f;
	_color.y = (std::rand() % 25) / 25.0f;
	_color.z = (std::rand() % 25) / 25.0f;
}

void Block::move(glm::vec2 direction, float speed)
{
	glm::vec2 move = glm::normalize(direction) * speed;
	_position.x += move.x;
	_position.y += move.y;
}
