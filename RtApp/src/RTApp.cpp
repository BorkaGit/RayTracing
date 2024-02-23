#include "Walnut/Application.h"
#include "Walnut/EntryPoint.h"
#include "Walnut/Image.h"
#include "Walnut/Timer.h"
#include "Renderer.h"
#include "Camera.h"
#include "User.h"
#include "glm/gtc/type_ptr.hpp"

using namespace Walnut;

class ExampleLayer : public Walnut::Layer
{
public:

	ExampleLayer()
		: MyCamera(45.0f, 0.1f, 100.0f)
	{

		Material& pinkSphere = MyScene.Materials.emplace_back();
		pinkSphere.Albedo = { 1.0f, 0.0f, 1.0f };

		Material& blueSphere = MyScene.Materials.emplace_back();
		blueSphere.Albedo = { 0.2f, 0.3f, 1.0f };

		{
			Sphere FirstSphere;
			FirstSphere.Position = { 0.0f, 0.0f, 0.0f };
			FirstSphere.Radius = 0.5f;
			FirstSphere.MaterialIndex = 0;
			MyScene.Spheres.push_back(FirstSphere);
		}

		{
			Sphere SecondSphere;
			SecondSphere.Position = { 1.0f, 0.0f, -5.0f };
			SecondSphere.Radius = 1.5f;
			SecondSphere.MaterialIndex = 1;
			MyScene.Spheres.push_back(SecondSphere);
		}
	}

	virtual void OnUpdate(float ts) override
	{
		MyCamera.OnUpdate(ts);
	}

	virtual void OnUIRender() override
	{
		ImGui::Begin("Settings");
		ImGui::Text("Last render: %.3fms", LastRenderTime);
		if (ImGui::Button("Render"))
		{
			Render();
		}
		if (ImGui::Button("Create a new sphere"))
		{
			Sphere NewSphere;
			MyScene.Spheres.push_back(NewSphere);
		}

		if (ImGui::Button("Create a new material"))
		{
			Material NewMaterial;
			MyScene.Materials.push_back(NewMaterial);
		}

		ImGui::ColorEdit3("SkyColor", glm::value_ptr(MyScene.SkyColor));

		ImGui::End();

		ImGui::Begin("Scene");

		for (size_t i = 0; i < MyScene.Spheres.size(); ++i)
		{
			ImGui::PushID(i);

			Sphere& MySphere = MyScene.Spheres[i];
			ImGui::DragFloat3("Position", glm::value_ptr(MySphere.Position), 0.1f);
			ImGui::DragFloat("Radius", &MySphere.Radius, 0.1f);
			ImGui::DragInt("Material", &MySphere.MaterialIndex, 1.0f, 0, (int)MyScene.Materials.size() - 1);

			if (ImGui::Button("Delete the Sphere"))
			{
				MyScene.Spheres.erase(MyScene.Spheres.begin() + i);
			}

			ImGui::Separator();

			ImGui::PopID();
		}

		ImGui::End();

		ImGui::Begin("Materials");

		for (size_t i = 0; i < MyScene.Materials.size(); ++i)
		{
			ImGui::PushID(i);

			Material& MyMaterial = MyScene.Materials[i];
			ImGui::Text("Material Index : %d", i);
			ImGui::ColorEdit3("Albedo", glm::value_ptr(MyMaterial.Albedo));
			ImGui::DragFloat("Roughness", &MyMaterial.Roughness, 0.05f, 0.0f, 1.0f);

			if (ImGui::Button("Delete the Material"))
			{
				MyScene.Materials.erase(MyScene.Materials.begin() + i);
			}

			ImGui::Separator();

			ImGui::PopID();
		}

		ImGui::End();

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("Viewport");

		ViewportWidth = ImGui::GetContentRegionAvail().x;
		ViewportHeight = ImGui::GetContentRegionAvail().y;

		auto FinalImage = MainRenderer.GetFinalImage();
		if (FinalImage)
			ImGui::Image(FinalImage->GetDescriptorSet(), { (float)FinalImage->GetWidth(), (float)FinalImage->GetHeight() },ImVec2(0, 1), ImVec2(1,0));

		ImGui::End();
		ImGui::PopStyleVar();

		Render();
	}

	void Render()
	{
		Timer timer;

		MainRenderer.OnResize(ViewportWidth, ViewportHeight);
		MyCamera.OnResize(ViewportWidth, ViewportHeight);
		MainRenderer.Render(MyScene,MyCamera);

		LastRenderTime = timer.ElapsedMillis();
	}
private:
	Renderer MainRenderer;
	Camera MyCamera;
	Scene MyScene;
	uint32_t ViewportWidth = 0, ViewportHeight = 0;

	float LastRenderTime = 0.0f;

	
};

Walnut::Application* Walnut::CreateApplication(int argc, char** argv)
{
	Walnut::ApplicationSpecification spec;
	spec.Name = "Ray Tracing";

	Walnut::Application* app = new Walnut::Application(spec);
	app->PushLayer<ExampleLayer>();
	app->SetMenubarCallback([app]()
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Exit"))
			{
				app->Close();
			}
			ImGui::EndMenu();
		}
	});
	return app;
}