#include "RasterizerMain.hpp"
#include "RayTracer.hpp"
#define mode 0 
//mode切换简易光追（学习原理用具体见自定义srp）/光栅化
int main(int argc, char* argv[]) {
#if mode==0 
	RunRasterizer();
#elif mode==1
	RunRayT();
#endif
	return 0;
}