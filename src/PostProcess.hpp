#pragma once
#include <vector>
#include <cmath>
#include <algorithm> 
#include "Maths.hpp" 
#include "Platform.hpp" 
class PostProcess {
public:
	static Vec3 GetHDR(const Platform& plt,const std::vector<Vec3>& bloomB,int x,int y,int rw) {
		int idx = y * rw + x;
		if (idx < 0 || idx >= bloomB.size()) return Vec3(0, 0, 0);
		Vec3 bcolor = bloomB[idx];
		Vec3 color = plt.hdrbuffer[idx];
		//调节bloom
		return color + bcolor * 0.5f;
	}

	//高光
	static std::vector<Vec3> ExBright(const std::vector<Vec3>& src,float threshold) {
		std::vector<Vec3> dest=src;
		for (int i = 0; i < src.size(); i++) {
			float bright = src[i].x * 0.2126f + src[i].y * 0.7152f + src[i].z * 0.0722f;
			if (bright > threshold) {
				dest[i] = src[i];
			}
			else {
				dest[i] = Vec3(0, 0, 0);
			}
		}
		return dest;
	}
	//blur
	static void Blur(std::vector<Vec3>& data, int w, int h, int passes) {
		std::vector<Vec3> temp = data;
		for (int p = 0; p < passes; p++) {

			//迭代模糊（低效具体bloom处理见srp）
			for (int y = 0; y < h; y++) {
				for (int x = 1; x < w - 1; x++) {
					temp[y * w + x] = (data[y * w + (x - 1)] + data[y * w + x] + data[y * w + (x + 1)]) * 0.3333f;
				}
			}
			data = temp;
			for (int y = 1; y < h - 1; y++) {
				for (int x = 0; x < w; x++) {
					temp[y * w + x] = (data[(y - 1) * w + x] + data[y * w + x] + data[(y + 1) * w + x]) * 0.3333f;
				}
			}
			data = temp;
		}
	}

    //钳制，压暗高光
	static Vec3 ACES(Vec3 x) {
		float a = 2.51f, b = 0.03f, c = 2.43f, d = 0.59f, e = 0.14f;
		Vec3 num = x * (x * a + Vec3(b, b, b));
		Vec3 den = x * (x * a + Vec3(d, d, d)) + Vec3(e, e, e);
		return num / den;
	}

	static void Resolve(Platform& plt) {
		int w = plt.GetWidth();
		int h = plt.GetHeight();
		int rw = plt.GetreW();
		int rh = plt.GetreH();

		std::vector<Vec3> bloom = ExBright(plt.hdrbuffer, 0.7f);
		Blur(bloom, rw, rh, 6);
		//合成
		for (int y = 0; y < h; y++) {
			for (int x = 0; x < w; x++) {
			
			int sX = x * 2;
			int sY = y * 2;
			Vec3 n1 = GetHDR(plt, bloom, sX, sY, rw);
			Vec3 n2 = GetHDR(plt, bloom, sX + 1, sY, rw);
			Vec3 n3 = GetHDR(plt, bloom, sX, sY + 1, rw);
			Vec3 n4 = GetHDR(plt, bloom, sX + 1, sY + 1, rw);

			Vec3 avgC = (n1 + n2 + n3 + n4) * 0.25f;
			avgC = ACES(avgC);
			avgC = Vec3::Pow(avgC, 1.0f / 2.2f);
			//Vec3 color = plt.hdrbuffer[i];
			avgC = Vec3::Clamp(avgC, 0.0f, 1.0f);
			float gray = avgC.Dot(Vec3(0.3, 0.59, 0.11));
			//饱和度
			avgC= Vec3::Lerp(Vec3(gray, gray, gray), avgC, 1.2f);

			uint8_t r = (uint8_t)(avgC.x * 255);
			uint8_t g = (uint8_t)(avgC.y * 255);
			uint8_t b = (uint8_t)(avgC.z * 255);

			plt.framebuffer[y * w + x] = (255 << 24) | (r << 16) | (g << 8) | b;
			
			}
		}
	}
};