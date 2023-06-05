#pragma once

#include "Walnut/Image.h"
#include <memory>

class Renderer
{
public:
	Renderer() = default;

	void Resize(uint32_t width, uint32_t height);
	void Render();
	
private:
	std::shared_ptr<Walnut::Image> image;
	uint32_t* pixels = nullptr; // The pixel array
	
};

