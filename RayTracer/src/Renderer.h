#pragma once

#include "Walnut/Image.h"
#include <memory>

// glm includes
#include <glm/glm.hpp>
#include "Camera.h"
#include "Ray.h"

#include "Scene.h"


typedef uint32_t uint32;
typedef uint32_t pixel;
typedef std::shared_ptr<Walnut::Image> ImagePtr;


class Renderer
{
public:

	struct Settings
	{
		bool Accumulate = true;
	};

	Renderer() = default;

	void Render(const Scene& scene, const Camera& camera);

	void Resize(uint32 width, uint32 height);

	auto GetImage() const { return image; }

	void ResetFrameIndex() { frameIndex = 1; }
	Settings& GetSettings() { return settings; }
	
private:

	struct HitPayload
	{
		float HitDistance = 0;
		glm::vec3 HitPosition;
		glm::vec3 HitNormal;

		uint32 ObjectIndex = 0;
	};


	glm::vec4 RayGen(uint32 x, uint32 y);

	 HitPayload TraceRay(const Ray& ray);
	 HitPayload ClosestHit(const Ray& ray, float hitDistance, uint32& objectIndex);
	 HitPayload Miss(const Ray& ray);

	const Scene* activeScene = nullptr;
	const Camera* activeCamera = nullptr;

	
	ImagePtr image;
	pixel* pixels = nullptr; // The pixel array
	glm::vec4* accumulationData = nullptr; // The accumulation buffer

	uint32 frameIndex = 1;
	Settings settings;
	
};

