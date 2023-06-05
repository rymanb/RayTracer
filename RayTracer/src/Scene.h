#pragma once

#include "glm/glm.hpp"
#include <vector>

struct Material
{
	glm::vec3 Albedo{1.0f};
	float Roughness = 1.0f;
	float Metallic = 0.0f;
};

struct Sphere
{
	glm::vec3 Position{ 0 };
	float Radius = 0.5f;
	int MaterialIndex;
};

struct Scene
{
	std::vector<Sphere> Objects;
	std::vector<Material> Materials;
};
