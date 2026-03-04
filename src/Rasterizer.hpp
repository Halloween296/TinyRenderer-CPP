#pragma once
#include "Platform.hpp"
#include "Maths.hpp"
#include "Texture.hpp"
#include <algorithm>

struct ScreenVertex {
	Vec4 pos;
	Vec2 uv;
};
//判断是否在内部全左边
inline float EdgeF(const Vec2& a,const Vec2&b,const Vec2& p) {
	//叉乘(p.x-a.x)+(p.y-a.y)  x  (b.x-a.x)+(b.y-a.y)
	return (p.x - a.x)* (b.y - a.y) - (p.y - a.y) * (b.x - a.x);

}
class Rasterizer {
public:
	Rasterizer(Platform& platform) : platform(platform) {}
	
	//重心坐标
	static Vec3 cofg(Vec2 a,Vec2 b,Vec2 c,Vec2 p){
		Vec3 u = Vec3(c.x - a.x, b.x - a.x, a.x - p.x).Cross(Vec3(c.y - a.y, b.y - a.y, a.y - p.y));
		if (std::abs(u.z) < 1e-2)return Vec3(-1, 1, 1);
		return Vec3(1.0f - (u.x + u.y) / u.z, u.y / u.z, u.x / u.z);
	
	}


	void DrawT(const ScreenVertex& v0,const ScreenVertex& v1,const ScreenVertex& v2,const Texture* tex,float intensity) {
		//得到包围盒
		int minx = std::max(0, (int)std::min({ v0.pos.x,v1.pos.x,v2.pos.x }));
		int miny = std::max(0, (int)std::min({ v0.pos.y,v1.pos.y,v2.pos.y }));
		int maxx = std::min(platform.GetreW() - 1, (int)std::max({v0.pos.x,v1.pos.x,v2.pos.x}));
		int maxy = std::min(platform.GetreH() - 1, (int)std::max({v0.pos.y,v1.pos.y,v2.pos.y}));
		
		float W0 = 1.0f / v0.pos.w;
		float W1 = 1.0f / v1.pos.w;
		float W2 = 1.0f / v2.pos.w;

		//包围盒遍历
		for (int y = miny; y < maxy; y++) {
			for (int x = minx; x < maxx ;x++) {
			 //拿中点
				Vec2 p(x+0.5f,y+0.5f);
				Vec3 bc = cofg(Vec2(v0.pos.x, v0.pos.y), Vec2(v1.pos.x, v1.pos.y), Vec2(v2.pos.x, v2.pos.y), p);
				//-1e-4
				const float ex = -0.01f;
				if (bc.x >= ex && bc.y >= ex && bc.z >= ex) {
				     //矫正
					float cd = 1.0f / (bc.x * W0 + bc.y * W1 + bc.z * W2);


					//ztest
					int idx = y * platform.GetWidth() + x;
					if (cd < platform.zBuffer[idx]) {
						platform.zBuffer[idx] = cd;

						float u = (v0.uv.x * W0 * bc.x + v1.uv.x * W1 * bc.y + v2.uv.x * W2 * bc.z) * cd;
						float v = (v0.uv.y * W0 * bc.x + v1.uv.y * W1 * bc.y + v2.uv.y * W2 * bc.z) * cd;
						uint32_t color = 0xFF00FF00;
						if (tex) {
							color = tex->Sample(u, v);
						}
						//0-255->0-1.0
						float r = ((color >> 16) & 0xFF) / 255.0f;
						float g = ((color >> 8) & 0xFF) / 255.0f;
						float b = (color & 0xFF) / 255.0f;

						Vec3 hdrColor(r, g, b);
						hdrColor = hdrColor * 1.0f*intensity;

						platform.PutPixelHDR(x, y, hdrColor);
					}

				}
			}
		}
	}
private:
	Platform& platform;
};