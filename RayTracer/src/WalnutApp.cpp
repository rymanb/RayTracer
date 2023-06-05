#include "Walnut/Application.h"
#include "Walnut/EntryPoint.h"

#include "Walnut/Image.h"
#include "Walnut/Timer.h"

#include "Renderer.h"
#include "Camera.h"

using namespace Walnut;

class ExampleLayer : public Walnut::Layer
{
public:

	ExampleLayer() : camera(45.0f, 0.1f, 100.0f)
	{
		Material& pink = scene.Materials.emplace_back();
		pink.Albedo = glm::vec3(1.0f, 0.0f, 1.0f);
		pink.Roughness = 0.0f;

		Material& blue = scene.Materials.emplace_back();
		blue.Albedo = glm::vec3(0.0f, 0.0f, 1.0f);
		blue.Roughness = 0.1f;
		
		
		scene.Objects.push_back({ glm::vec3(0.0f, 0.0f, 0.0f), 1.0f, 0 });

		scene.Objects.push_back({ glm::vec3(0.0f, -101.0f, 0.0f), 100.0f, 1 });
	}

	virtual void OnUpdate(float ts) override
	{
		if (camera.OnUpdate(ts))
		{
			renderer.ResetFrameIndex();
		}


	}

	virtual void OnUIRender() override
	{
		ImGui::Begin("Settings");
		ImGui::Text("Last Render Time: %.3fms", lastRenderTime);
		if (ImGui::Button("Render"))
		{
			Render();
		}
		ImGui::Checkbox("Accumulate", &renderer.GetSettings().Accumulate);
		if (ImGui::Button("Reset"))
		{
			renderer.ResetFrameIndex();
		}

		ImGui::End();		
		
		ImGui::Begin("Scene");
		
		for (size_t i = 0; i < scene.Objects.size(); i++)
		{

			ImGui::PushID(i);
			Sphere& sphere = scene.Objects[i];
			ImGui::Text("Sphere");
			ImGui::DragFloat3("Position", &sphere.Position.x, 0.1f);
			ImGui::DragFloat("Radius", &sphere.Radius, 0.1f);
			ImGui::DragInt("Material Index", &sphere.MaterialIndex, 1.0f, 0, scene.Materials.size() - 1);
			ImGui::PopID();
		}

		for (size_t i = 0; i < scene.Materials.size(); i++)
		{
			ImGui::PushID(i);

			Material& mat = scene.Materials[i];
			ImGui::ColorEdit3("Color", &mat.Albedo.x);
			ImGui::DragFloat("Roughness", &mat.Roughness, 0.05f, 0.0f, 1.0f);
			ImGui::DragFloat("Metallic", &mat.Metallic, 0.05f, 0.0f, 1.0f);
			ImGui::PopID();
		}

		ImGui::End();

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
		// view image
		ImGui::Begin("ViewPort");

		// get viewport size
		auto& region = ImGui::GetContentRegionAvail();
		viewPortWidth = region.x; viewPortHeight = region.y;



		//{
		//	static float rotation = 0.0f;
		//	// rotate object 1 around origin
		//	Sphere& sphere = scene.objects[0];
		//	sphere.position.x = cosf(rotation) * 2.0f;
		//	sphere.position.z = sinf(rotation) * 2.0f;
		//	rotation += 0.01f;
		//	
		//	
		//	
		//	
		//}

		auto image = renderer.GetImage();
		if (image)
			ImGui::Image(image->GetDescriptorSet(), { (float)image->GetWidth(), (float)image->GetHeight() }, { 0, 1 }, { 1, 0 });
		
		ImGui::End();

		ImGui::PopStyleVar();

		Render();
	}



	void Render()
	{
		Timer timer;

		renderer.Resize(viewPortWidth, viewPortHeight);
		camera.OnResize(viewPortWidth, viewPortHeight);
		renderer.Render(scene, camera);



		
		// update timer
		lastRenderTime = timer.ElapsedMillis();
	}

private:
	Renderer renderer;
	Camera camera;
	Scene scene;
	
	// The size of the viewport
	uint32_t viewPortWidth = 0,  viewPortHeight = 0;

	// timer
	float lastRenderTime = 0;
};

Walnut::Application* Walnut::CreateApplication(int argc, char** argv)
{
	Walnut::ApplicationSpecification spec;
	spec.Name = "Ray Tracer";

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