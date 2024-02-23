#include "Camera.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

#include "Walnut/Input/Input.h"

using namespace Walnut;

Camera::Camera(float VerticalFOV, float NearClip, float FarClip)
	: VerticalFOV(VerticalFOV), NearClip(NearClip), FarClip(FarClip)
{
	ForwardDirection = glm::vec3(0, 0, -1);
	Position = glm::vec3(0, 0, 3);
}

void Camera::OnUpdate(float ts)
{
	glm::vec2 MousePos = Input::GetMousePosition();
	glm::vec2 Delta = (MousePos - LastMousePosition) * 0.002f;
	LastMousePosition = MousePos;

	if (!Input::IsMouseButtonDown(MouseButton::Right))
	{
		Input::SetCursorMode(CursorMode::Normal);
		return;
	}

	Input::SetCursorMode(CursorMode::Locked);

	bool Moved = false;

	constexpr glm::vec3 UpDirection(0.0f, 1.0f, 0.0f);
	glm::vec3 RightDirection = glm::cross(ForwardDirection, UpDirection);

	float Speed = 5.0f;

	// Movement
	if (Input::IsKeyDown(KeyCode::W))
	{
		Position += ForwardDirection * Speed * ts;
		Moved = true;
	}
	else if (Input::IsKeyDown(KeyCode::S))
	{
		Position -= ForwardDirection * Speed * ts;
		Moved = true;
	}
	if (Input::IsKeyDown(KeyCode::A))
	{
		Position -= RightDirection * Speed * ts;
		Moved = true;
	}
	else if (Input::IsKeyDown(KeyCode::D))
	{
		Position += RightDirection * Speed * ts;
		Moved = true;
	}
	if (Input::IsKeyDown(KeyCode::Q))
	{
		Position -= UpDirection * Speed * ts;
		Moved = true;
	}
	else if (Input::IsKeyDown(KeyCode::E))
	{
		Position += UpDirection * Speed * ts;
		Moved = true;
	}

	// Rotation
	if (Delta.x != 0.0f || Delta.y != 0.0f)
	{
		float PitchDelta = Delta.y * GetRotationSpeed();
		float YawDelta = Delta.x * GetRotationSpeed();

		glm::quat q = glm::normalize(glm::cross(glm::angleAxis(-PitchDelta, RightDirection),
			glm::angleAxis(-YawDelta, glm::vec3(0.f, 1.0f, 0.0f))));
		ForwardDirection = glm::rotate(q, ForwardDirection);

		Moved = true;
	}

	if (Moved)
	{
		RecalculateView();
		RecalculateRayDirections();
	}
}

void Camera::OnResize(uint32_t Width, uint32_t Height)
{
	if (Width == ViewportWidth && Height == ViewportHeight)
		return;

	ViewportWidth = Width;
	ViewportHeight = Height;

	RecalculateProjection();
	RecalculateRayDirections();
}

float Camera::GetRotationSpeed()
{
	return 0.3f;
}

void Camera::RecalculateProjection()
{
	Projection = glm::perspectiveFov(glm::radians(VerticalFOV), (float)ViewportWidth, (float)ViewportHeight, NearClip, FarClip);
	InverseProjection = glm::inverse(Projection);
}

void Camera::RecalculateView()
{
	View = glm::lookAt(Position, Position + ForwardDirection, glm::vec3(0, 1, 0));
	InverseView = glm::inverse(View);
}

void Camera::RecalculateRayDirections()
{
	RayDirections.resize(ViewportWidth * ViewportHeight);

	for (uint32_t y = 0; y < ViewportHeight; y++)
	{
		for (uint32_t x = 0; x < ViewportWidth; x++)
		{
			glm::vec2 Coord = { (float)x / (float)ViewportWidth, (float)y / (float)ViewportHeight };
			Coord = Coord * 2.0f - 1.0f; // -1 -> 1

			glm::vec4 Target = InverseProjection * glm::vec4(Coord.x, Coord.y, 1, 1);
			glm::vec3 RayDirection = glm::vec3(InverseView * glm::vec4(glm::normalize(glm::vec3(Target) / Target.w), 0)); // World space
			RayDirections[x + y * ViewportWidth] = RayDirection;
		}
	}
}
