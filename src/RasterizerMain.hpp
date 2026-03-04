#include "Platform.hpp"
#include"Rasterizer.hpp"
#include "Maths.hpp"
#include "Mesh.hpp"
#include "Pipeline.hpp"
#include "PostProcess.hpp"

int RunRasterizer() {
	const int W = 640;
	const int H = 480;
	Platform platform("111", W, H);
	Rasterizer rasterizer(platform);
	Pipeline pipline(platform.GetreW(),platform.GetreH());
	//绝对路径可改
	Texture myTex("E:\\Render\\RetroRenderer\\assets\\phone.png");

	Mesh mesh;
	if (!mesh.LoadObj("E:\\Render\\RetroRenderer\\assets\\phone.obj")) { std::cout << "Load Failed!" << std::endl; system("pause"); return -1; }
	float angle = 0.0f;
	while (platform.ProcessInput()) {
		platform.ClearBuffer();
		angle += 0.002f;

		pipline.SetModel(Mat4::RotateY(angle));
		pipline.SetView(Mat4::LookAt(Vec3(0, 0, 5), Vec3(0, 0, 0), Vec3(0, 1, 0)));
		pipline.SetProjection(Mat4::Perspective(1.57f, (float)W / H, 1.0f, 20.0f));

		pipline.DrawMesh(mesh, rasterizer, &myTex);
		PostProcess::Resolve(platform);

		platform.SwapBuffers();
	}
	return 0;
}