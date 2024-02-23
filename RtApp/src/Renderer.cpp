#include "Renderer.h"
#include "Walnut/Random.h"

namespace Utilities {

	static uint32_t ConvertToRGBA(const glm::vec4& Color)
	{
		uint8_t r = (uint8_t)(Color.r * 255.0f);
		uint8_t g = (uint8_t)(Color.g * 255.0f);
		uint8_t b = (uint8_t)(Color.b * 255.0f);
		uint8_t a = (uint8_t)(Color.a * 255.0f);

		uint32_t Result = (a << 24) | (b << 16) | (g << 8) | r;
		return Result;
	}

}

void Renderer::OnResize(uint32_t Width, uint32_t Height)
{
	if (FinalImage)
	{
		if (FinalImage->GetWidth() == Width && FinalImage->GetHeight() == Height)
		{
			return;
		}

		FinalImage->Resize(Width,Height);
	}
	else
	{
		FinalImage = std::make_shared<Walnut::Image>(Width, Height, Walnut::ImageFormat::RGBA);
	}

	delete[] ImageData;
	ImageData = new uint32_t[Width * Height];
}


void Renderer::Render(const Scene& MyScene, const Camera& MyCamera)
{
	//render per pixel

	ActiveScene = &MyScene;
	ActiveCamera = &MyCamera;

	const glm::vec3& RayOrigin = MyCamera.GetPosition();

	for (uint32_t y = 0; y < FinalImage->GetHeight(); y++)
	{
		for (uint32_t x = 0; x < FinalImage->GetWidth(); x++)
		{
			const glm::vec3& RayDirection = MyCamera.GetRayDirections()[x + y * FinalImage->GetWidth()];

			glm::vec4 PixelColor = PerPixel(x, y);
			PixelColor = glm::clamp(PixelColor, glm::vec4(0.0f), glm::vec4(1.0f));
			ImageData[x + y * FinalImage->GetWidth()] = Utilities::ConvertToRGBA(PixelColor);
		}
	}

	FinalImage->SetData(ImageData);
}

glm::vec4 Renderer::PerPixel(uint32_t x, uint32_t y)
{
	Ray MyRay;
	MyRay.Origin = ActiveCamera->GetPosition();
	MyRay.Direction = ActiveCamera->GetRayDirections()[x + y * FinalImage->GetWidth()];

	glm::vec3 PixelColor(0.0f);
	float Multiplier = 1.0f;

	int Bounces = 5;
	for (int i = 0; i < Bounces; i++)
	{
		Renderer::HitPayload Payload = TraceRay(MyRay);
		if (Payload.HitDistance < 0.0f)
		{
			glm::vec3 SkyColor = ActiveScene->SkyColor;
			PixelColor += SkyColor * Multiplier;
			break;
		}

		glm::vec3 LightDir = glm::normalize(glm::vec3(-1, -1, -1));
		float LightIntensity = glm::max(glm::dot(Payload.WorldNormal, -LightDir), 0.0f); // == cos(angle)

		const Sphere& MySphere = ActiveScene->Spheres[Payload.ObjectIndex];
		const Material& MyMaterial = ActiveScene->Materials[MySphere.MaterialIndex];
		glm::vec3 SphereColor = MyMaterial.Albedo;
		SphereColor *= LightIntensity;
		PixelColor += SphereColor * Multiplier;

		Multiplier *= 0.5f;

		MyRay.Origin = Payload.WorldPosition + Payload.WorldNormal * 0.0001f;
		MyRay.Direction = glm::reflect(MyRay.Direction, Payload.WorldNormal + MyMaterial.Roughness * Walnut::Random::Vec3(-0.5f, 0.5f));
	}

	return glm::vec4(PixelColor, 1.0f);
}

Renderer::HitPayload Renderer::TraceRay(const Ray& MyRay)
{
	// RayDirection = glm::normalize(RayDirection);

	// (bx^2 + by^2)t^2 + 2t(axbx+ ayby) + (ax^2 + ay^2 - r^2) = 0
	// ray formula = a + bt , a - origin, b - direction, t - hit distance
	
	// ax^2 -bx + c = 0

	int ClosestSphere = -1;
	float HitDistance = FLT_MAX;

	for (size_t i = 0; i < ActiveScene->Spheres.size(); ++i)
	{
		const Sphere& MySphere = ActiveScene->Spheres[i];
		glm::vec3 NewOrigin = MyRay.Origin - MySphere.Position;

		// RayDirection.x * RayDirection.x + RayDirection.y * RayDirection.y + RayDirection.z * RayDirection.z   == dotproduct(RayDirection,RayDirection)
		float a = glm::dot(MyRay.Direction, MyRay.Direction);
		float b = 2.0f * glm::dot(NewOrigin, MyRay.Direction);
		float c = glm::dot(NewOrigin, NewOrigin) - MySphere.Radius * MySphere.Radius;

		// discriminant b^2 - 4ac

		float Discriminant = b * b - 4.0f * a * c;

		if (Discriminant < 0.0f)
		{
			continue;
		}

		// (-b +- sqrt(discriminant)) / 2a

		//float t0 = (-b + glm::sqrt(Discriminant)) / (2.0f * a); // Second hit distance 
		float ClosestT = (-b - glm::sqrt(Discriminant)) / (2.0f * a);
		if (ClosestT > 0.0f && ClosestT < HitDistance)
		{
			HitDistance = ClosestT;
			ClosestSphere = (int)i;
		}
	}

	if (ClosestSphere < 0)
	{
		return Miss(MyRay);
	}

	
	return ClosestHit(MyRay, HitDistance, ClosestSphere);
}

Renderer::HitPayload Renderer::ClosestHit(const Ray& MyRay, float HitDistance, int ObjectIndex)
{
	Renderer::HitPayload Payload;
	Payload.HitDistance = HitDistance;
	Payload.ObjectIndex = ObjectIndex;

	const Sphere& ClosestSphere = ActiveScene->Spheres[ObjectIndex];

	glm::vec3 Origin = MyRay.Origin - ClosestSphere.Position;
	Payload.WorldPosition = Origin + MyRay.Direction * HitDistance;
	Payload.WorldNormal = glm::normalize(Payload.WorldPosition);

	Payload.WorldPosition += ClosestSphere.Position;

	return Payload;
}

Renderer::HitPayload Renderer::Miss(const Ray& MyRay)
{
	Renderer::HitPayload Payload;
	Payload.HitDistance = -1.0f;
	return Payload;
}