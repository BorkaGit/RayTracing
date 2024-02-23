#pragma once

#include <glm/glm.hpp>

struct Ray
{
	glm::vec3 Origin;
	glm::vec3 Direction;

public:
	glm::vec3 GetOrigin() const { return Origin; };
	glm::vec3 GetDirection() const { return Direction; };
};