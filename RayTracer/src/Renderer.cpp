#include "Renderer.h"
#include "Walnut/Input/Input.h"
#include "Walnut/Input/KeyCodes.h"
#include "Walnut/Random.h"
#define _USE_MATH_DEFINES
#include <math.h>

namespace Utility
{
	static uint32 VectorToRGBA(const glm::vec4& color)
	{
		uint32 r = static_cast<uint32>(color.r * 255.0f);
		uint32 g = static_cast<uint32>(color.g * 255.0f);
		uint32 b = static_cast<uint32>(color.b * 255.0f);
		uint32 a = static_cast<uint32>(color.a * 255.0f);

		return (a << 24) | (b << 16) | (g << 8) | r;
	}
}

void Renderer::Render(const Scene& scene, const Camera& camera)
{

	activeScene = &scene;
	activeCamera = &camera;

	
	if (frameIndex == 1)
		memset(accumulationData, 0, image->GetWidth() * image->GetHeight() * sizeof(glm::vec4));
	
	// set the pixel array to the image
	for (uint32 y = 0; y < image->GetHeight(); y++)
	{
		for (uint32 x = 0; x < image->GetWidth(); x++)
		{
			auto color = RayGen(x, y);
			

			accumulationData[x + y * image->GetWidth()] += color;
			
			color = accumulationData[x + y * image->GetWidth()];
			color /= (float)frameIndex;
			
			color = glm::clamp(color, glm::vec4(0.0f), glm::vec4(1.0f));

			pixels[x + y * image->GetWidth()] = Utility::VectorToRGBA(color);
		}
	}

	image->SetData(pixels);

	if (settings.Accumulate)
		frameIndex++;
	else
		frameIndex = 1;
	
}

// phong reflection model returns reflected ray direction
glm::vec3 PhongReflection(glm::vec3 normal, glm::vec3 oiginalDirection, float roughness)
{
	using namespace Walnut;
	// calculate reflected ray direction
	glm::vec3 reflectedDirection = glm::reflect(oiginalDirection, normal);

	// calculate random direction
	glm::vec3 randomDirection = glm::normalize(glm::vec3(Random::Float(), Random::Float(), Random::Float()));

	// calculate new direction
	glm::vec3 newDirection = glm::normalize(reflectedDirection + randomDirection * roughness);

	return newDirection;
}


glm::vec4 Renderer::RayGen(uint32 x, uint32 y)
{
	Ray ray;
	ray.origin = activeCamera->GetPosition();
	ray.direction = activeCamera->GetRayDirections()[x + y * image->GetWidth()];

	glm::vec3 finalColor(0.0f);

	int maxBounces = 5;

	float mult = 1.0f;
	
	for (int i = 0; i < maxBounces; i++)
	{
		auto hit = TraceRay(ray);

		if (hit.HitDistance == -1)
		{
			glm::vec3 sky = glm::vec3(0.6f, 0.7f, 0.9f);
			finalColor += mult * sky;
			break;
		}


		glm::vec3 lightDir = glm::normalize(glm::vec3(-1, -1, -1));

		float d = glm::max(glm::dot(hit.HitNormal, -lightDir), 0.0f); // same as cos(angle)

		auto& object = activeScene->Objects[hit.ObjectIndex];

		auto& material = activeScene->Materials[object.MaterialIndex];

		glm::vec3 color = material.Albedo;
		color *= d;

		finalColor += color * mult;

		mult *= 0.5f;

		ray.origin = hit.HitPosition + hit.HitNormal * 0.0001f;
		ray.direction = glm::reflect(ray.direction, hit.HitNormal + material.Roughness * Walnut::Random::Vec3(-0.5f, 0.5f));
		
		//ray.direction = glm::normalize(VCosineSampleHemisphere(material.Roughness) + hit.HitNormal);
		
		// use phong reflection model to get the reflected ray
		//ray.direction = glm::normalize(PhongReflection(hit.HitNormal, ray.direction, material.Roughness));

		
		
		

	}
	


	return glm::vec4(finalColor, 1.0f);

}

inline Renderer::HitPayload Renderer::TraceRay(const Ray& ray)
{
	
	uint32 closestSphere = UINT32_MAX;
	float t = FLT_MAX;
	
	for (uint32 i = 0; i < activeScene->Objects.size(); i++)
	{

		const Sphere& sphere = activeScene->Objects[i];
		
		glm::vec3 origin = ray.origin - sphere.Position;

		float radius = sphere.Radius;

		float a = glm::dot(ray.direction, ray.direction);
		float b = 2.0f * glm::dot(origin, ray.direction);
		float c = glm::dot(origin, origin) - radius * radius;

		float dis = b * b - 4.0f * a * c;




		if (dis < 0)
			continue;
		// 2 solutions


		float t1 = (-b + glm::sqrt(dis)) / (2.0f * a);
		float t2 = (-b - glm::sqrt(dis)) / (2.0f * a);

		t1 = glm::min(t1, t2);
		if (t1 > 0.0f && t1 < t)
		{
			t = t1;
			closestSphere = i;
		}

	}

	if (closestSphere == UINT32_MAX)
		return Miss(ray);

	return ClosestHit(ray, t, closestSphere);
}

Renderer::HitPayload Renderer::ClosestHit(const Ray& ray, float hitDistance, uint32& objectIndex)
{
	const Sphere& closestSphere = activeScene->Objects[objectIndex];
	
	glm::vec3 origin = ray.origin - closestSphere.Position;

	glm::vec3 hitPoint = origin + ray.direction * hitDistance;

	// normal = pos - center
	glm::vec3 normal = glm::normalize(hitPoint);
	
	hitPoint += closestSphere.Position;



	return { hitDistance, hitPoint, normal, objectIndex };
}

Renderer::HitPayload Renderer::Miss(const Ray& ray)
{
	return { -1 };
}

void Renderer::Resize(uint32 width, uint32 height)
{
	if (image)
	{
		if (image->GetWidth() == width && image->GetHeight() == height)
			return;
		image->Resize(width, height);
	}
	else
	{
		image = std::make_shared<Walnut::Image>(width, height, Walnut::ImageFormat::RGBA);
	}
	
	delete[] pixels;
	pixels = new pixel[width * height];
	
	delete[] accumulationData;
	accumulationData = new glm::vec4[width * height];
	
}
