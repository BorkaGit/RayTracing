#pragma once

#include "Walnut/Image.h"
#include <glm/glm.hpp>
#include <memory>
#include "Ray.h"
#include "Camera.h"
#include "Scene.h"

class Renderer
{
public:
	Renderer() = default;

	void Render(const Scene& MyScene,const Camera& MyCamera);

	void OnResize(uint32_t Width, uint32_t Height);

	std::shared_ptr<Walnut::Image> GetFinalImage() const { return FinalImage; };

private:
	struct HitPayload
	{
		float HitDistance;
		glm::vec3 WorldPosition;
		glm::vec3 WorldNormal;

		int ObjectIndex;
	};

	glm::vec4 PerPixel(uint32_t x, uint32_t y); 

	HitPayload TraceRay(const Ray& ray);
	HitPayload ClosestHit(const Ray& ray, float hitDistance, int objectIndex);
	HitPayload Miss(const Ray& ray);

private:
	std::shared_ptr<Walnut::Image> FinalImage;

	const Scene* ActiveScene = nullptr;
	const Camera* ActiveCamera = nullptr;

	uint32_t* ImageData = nullptr;
};