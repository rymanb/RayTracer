#include "Renderer.h"

void Renderer::Render()
{
}

void Renderer::Resize(uint32_t width, uint32_t height)
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
	pixels = new uint32_t[width * height];
}
