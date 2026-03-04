#pragma once
#include "Maths.hpp"
#include "Mesh.hpp"
#include "Rasterizer.hpp"


class Pipeline {
public:
	Pipeline(int w, int h) :width(w), height(h) {}

	void SetModel(const Mat4& m) { model = m; }
	void SetView(const Mat4& m) { view = m; }
	void SetProjection(const Mat4& m) { proj = m; }

	void DrawMesh(const Mesh& mesh, Rasterizer& rasterizer, const Texture* tex) {
		Mat4 mvp = proj * view * model;

		for (const auto& tri : mesh.tri) {
			ScreenVertex v[3];
			for (int i = 0; i < 3; i++) {
				Vec3 rPos = (i == 0 ? tri.v0.pos : (i == 1 ? tri.v1.pos : tri.v2.pos));
				Vec2 rUV = (i == 0 ? tri.v0.uv : (i == 1 ? tri.v1.uv : tri.v2.uv));

				//mvp
				Vec4 c = mvp * Vec4(rPos.x, rPos.y, rPos.z, 1.0f);

				float w = c.w;
				if (w == 0) { w = 0.0001f; }
				float W = 1.0f / w;

				//ndc（-1，1）转换（0，w/h）
				v[i].pos.x = (c.x * W + 1.0f) * 0.5f * width;
				v[i].pos.y = (1.0f - c.y * W) * 0.5f * height;
				v[i].pos.z = c.z;
				v[i].pos.w = w;
				v[i].uv = rUV;
			}
			Vec3 lDir = Vec3(0.5f, 1.0f, 0.5f).Normalized();
		
			Vec3 p0 = v[0].pos.xyz();
			Vec3 p1 = v[1].pos.xyz();
			Vec3 p2 = v[2].pos.xyz();

			Vec3 edge1 = p1 - p0;
			Vec3 edge2 = p2 - p0;

			Vec3 normal = edge1.Cross(edge2).Normalized();
			//注释掉背面剔除了模型问题blender处理
			//if (normal.z > 0) {
					Vec3 wp0 = tri.v0.pos;
					Vec3 wp1 = tri.v1.pos;
					Vec3 wp2 = tri.v2.pos;
					Vec3 wNormal = (wp1 - wp0).Cross(wp2 - wp0).Normalized();

					//半兰伯特
					float diff = std::max(0.0f, wNormal.Dot(lDir) * 0.5f + 0.5f);
					float intensity = diff * 0.8f + 0.2f;
					rasterizer.DrawT(v[0], v[1], v[2], tex, intensity);
			//}
			
		}
	}

private:
	int width, height;
	Mat4 model, view, proj;

};