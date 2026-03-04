#pragma once
#include <vector>
#include <cstdint>
#include <cmath>
#include <string>
#include <iostream>
#define STB_IMAGE_IMPLEMENTATION 
#include "stb_image.h" 

class Texture {
public:
	int width, height;
	std::vector<uint32_t> pixels;

	Texture(int w, int h) :width(w), height(h) {
		pixels.resize(w * h);
		for (int y = 0; y < h; y++) {
			for (int x = 0; x < w; x++)
			{
				bool check = ((x / 32) + (y / 32)) % 2 == 0;
				pixels[y * w + x] = check ? 0xFFFFC0CB : 0xFFFFFFFF;
			} 
		}

	}

	Texture(const std::string& filename) {
		int channel;
		unsigned char* data = stbi_load(filename.c_str(), &width, &height, &channel, 4);
		if (!data) {
			std::cerr << "Failed to load texture: " << filename << std::endl;
			width = 1; height = 1;
			pixels.push_back(0xFFFF00FF);
			return;
		}
		pixels.resize(width * height);
		for (int i = 0; i < width * height; i++) {
			uint8_t r = data[i * 4 + 0];
			uint8_t g = data[i * 4 + 1];
			uint8_t b = data[i * 4 + 2];
			uint8_t a = data[i * 4 + 3];

			pixels[i] = (a << 24) | (r << 16) | (g << 8) | b;
		}
		stbi_image_free(data);
		std::cout << "Textureload" << std::endl;
	}

	uint32_t Sample(float u, float v)const {
		u = u - std::floor(u);
		v = v - std::floor(v);
		int x = static_cast<int>(u * (width - 1));
		int y = static_cast<int>(v * (height - 1));
		return pixels[y * width + x];
	}

};