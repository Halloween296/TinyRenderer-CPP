#include "Platform.hpp"
#include "Maths.hpp"
#include <vector>
#include <limits>
#include <algorithm>

struct Ray {
	Vec3 ori;
	Vec3 dir;
};

struct Hit {
	float t;
	Vec3 point;
	Vec3 normal;
	int objIn;
};

struct Sphere {
	Vec3 c;
	float radius;
	Vec3 color;
	float rge;
};
std::vector<Sphere> scene;
void InitS() {
	//漫反射
	scene.push_back({ Vec3(0, 0, -5), 1.0f, Vec3(1.0f, 0.6f, 0.7f), 1.0f });
	//反射
	scene.push_back({ Vec3(1.5f, 0, -4), 0.8f, Vec3(0.8f, 0.8f, 0.8f), 0.0f });
	//地面
	scene.push_back({ Vec3(0, -101, -5), 100.0f, Vec3(0.2f, 0.2f, 0.8f), 1.0f });
}

float HitSphere(const Ray& ray, const Sphere& sphere) {
	Vec3 oc = ray.ori - sphere.c;
	float a = ray.dir.Dot(ray.dir);
	float b = 2.0f * oc.Dot(ray.dir);
	float c = oc.Dot(oc) - sphere.radius * sphere.radius;
    float dis = b * b - 4 * a * c;

	//没到
	if (dis < 0) {
		return -1.0f;
	}
	else {
		return (-b - std::sqrt(dis)) / (2.0f * a);
	}
}

//场景遍历
Hit Trace(const Ray& ray) {
	Hit chit;
	chit.t = std::numeric_limits<float>::infinity();
	chit.objIn = -1;
	for (int i = 0; i < scene.size(); i++) {
		float t = HitSphere(ray, scene[i]);
		if (t > 0.001f && t < chit.t) {
			chit.t = t;
			chit.point = ray.ori + ray.dir * t;
			chit.normal = (chit.point - scene[i].c).Normalized();
			chit.objIn = i;
		}
	}
	return chit;
}

Vec3 PerPixel(const Ray& ray, int depth) {
	if (depth <= 0) {
		return Vec3(0, 0, 0);
	}
	Hit hitn = Trace(ray);
	if (hitn.objIn < 0) {
		float t = 0.5f * (ray.dir.y + 1.0f);
		return Vec3::Lerp(Vec3(1.0f, 1.0f, 1.0f), Vec3(0.5f, 0.7f, 1.0f), t);
		//return Vec3(1.0f, 1.0f, 1.0f) * (1.0f - t) + Vec3(0.5f, 0.7f, 1.0f) * t;
	}
	const Sphere& sp = scene[hitn.objIn];

	Vec3 LDir = Vec3(-1, 1, 1).Normalized();
	float LightI = std::max(0.0f, hitn.normal.Dot(LDir));

	//shadow
	Ray sRay = { hitn.point,LDir };
	Hit sHit = Trace(sRay);
	if (sHit.objIn >= 0) {
		LightI *= 0.1f;
	}
	Vec3 color = sp.color * LightI;
	//反射
	if (sp.rge < 0.5f) {
		Vec3 reDir = ray.dir - hitn.normal * 2.0f * ray.dir.Dot(hitn.normal);
		Ray reRay = { hitn.point,reDir.Normalized() };
		Vec3 reColor = PerPixel(reRay, depth - 1);
		color = color * 0.5f + reColor * 0.5f;
	}

	return color;
	}

void RunRayT() {
	Platform platform("Ray Tracer", 640, 480);
	InitS();
	float w = (float)platform.GetWidth();
	float h = (float)platform.GetHeight();
	float aspect = w / h;

	while (platform.ProcessInput()) {
		platform.ClearBuffer();
		for (int y = 0; y < platform.GetHeight(); y++) {
			for (int x = 0; x < platform.GetWidth(); x++) {

				float u = (x / w) * 2.0f - 1.0f;
				float v = (y / h) * 2.0f - 1.0f;
				u *= aspect;
				v *= -1;

				Ray ray;
				ray.ori = Vec3(0, 0, 0);
				ray.dir = Vec3(u, v, -1).Normalized();

				Vec3 color = PerPixel(ray, 3);

				color = Vec3::Pow(color, 1.0f / 2.2f);
				color = Vec3::Clamp(color, 0.0f, 1.0f);

				uint8_t r = (uint8_t)(color.x * 255);
				uint8_t g = (uint8_t)(color.y * 255);
				uint8_t b = (uint8_t)(color.z * 255);
				platform.framebuffer[y * platform.GetWidth() + x] = (255 << 24) | (r << 16) | (g << 8) | b;

			}
		}
		platform.SwapBuffers();
	}
}