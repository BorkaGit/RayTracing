#pragma once

#include <glm/glm.hpp>
#include <iostream>
#include <vector>

struct Material
{
	glm::vec3 Albedo{ 1.0f };
	float Roughness = 1.0f;

public:
	glm::vec3 GetAlbedo() const { return Albedo; };
	float GetRoughness() const { return Roughness; };
};

struct Sphere
{
	glm::vec3 Position{ 0.0f };
	float Radius = 0.5f;
	int MaterialIndex = 0;

public:
	glm::vec3 GetPosition() const { return Position; };
	float GetRadius() const { return Radius; };
	int GetMaterialIndex() const { return MaterialIndex; };
};

struct Scene
{
	glm::vec3 SkyColor{ 0.0f };

	std::vector<Sphere> Spheres;
	std::vector<Material> Materials;

public:
	glm::vec3 GetSkyColor() const { return SkyColor; };
	std::vector<Sphere> GetSpheres() const { return Spheres; };
	std::vector<Material> GetMaterials() const { return Materials; };
};