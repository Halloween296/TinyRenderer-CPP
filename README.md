# TinyRenderer-CPP
A 3D Software Rasterizer built from scratch in C++（3D 软光栅/模拟 GPU /C++）

不依赖图形api用c++从零实现的软光栅，项目实现了完整的3D渲染管线以及带后处理系统，呈现数码柔光效果

无后期处理：

![triRenderbefore](https://github.com/user-attachments/assets/2d256678-6eaf-49f7-a66b-2dbebe3961e5)

后期处理（Bloom 泛光 (阈值 0.6) + ACES 色调映射 + 饱和度增强 (基于亮度插值)）：

![triRender](https://github.com/user-attachments/assets/84bc46c3-09db-41db-95d6-801aa8cf337e)


注：
因为学习games101内容后想要通过实践去巩固理解才实现的这个，通过项目实现学习了包括mvp矩阵变换、重心插值（透视矫正）、zbuffer深度测试、透视矩阵就是把z写入w分量之后用齐次除法实现近大远小透视矫正就是抵消这个非线性变化插值前先除w之后再乘回来，了解了前向渲染比如移动端/urp管线和ue延迟渲染相比的一些痛点等虽然教程里用的是MSAA但是我为了省事和效果还是用SSAA抗锯齿所以用 Release 模式但是在我后续的srp自定义管线中还是用的MSAA抗锯齿因为它不像SSAA把Pixel Shader跑4遍，而是只在三角形边缘做多重采样开销小。


技术栈：

此渲染器运行在cpu上，覆盖了从几何处理到像素输出的完整周期

1、Maths.hpp：实现struct Vec2/3/4/Mat4 和常见图形学运算，支持基于行主序的矩阵乘法和齐次坐标变换

2、实现 Local -> World -> View (LookAt) -> Clip (Perspective) -> NDC -> Screen 的 5 大空间坐标变换

3、Mesh.hpp and Texture.hpp : 简单的obj格式解析和支持纹理

4、zbuffer非线性深度缓冲解决遮挡问题，背面剔除优化通过叉乘判断n朝向<0则剔除减少ps阶段计算量

5、集成stb_image支持漫反射贴图采样和UV Warp\Repeat

6、通过叉乘计算三角形面法线dot实现半兰伯特光照

7、SSAA 两倍分辨率超采样后卷积暴力消除锯齿

8、bloom基于亮度阈值提取高光部分与多次模糊（横纵）混合--SRP自定义管线改用金字塔方法减少drawcall

9、ACES色调映射防止亮部失真，bloom过亮问题通过GetHDR的返回值的bloom项乘以0.5降低



结构：
```text
RetroRenderer/
├── CMakeLists.txt     
├── assets/               #  .obj 模型与贴图
├── vendor/               # (SDL2, stb_image)
└── src/
    ├── main.cpp         
    ├── Platform.hpp     
    ├── Maths.hpp         # 数学库 (Vec2/3/4, Mat4, 矩阵变换)
    ├── Mesh.hpp          # 几何处理 (OBJ 加载、UV解析、三角化)
    ├── Texture.hpp       # 纹理管理与采样 (Wrap 模式、stbi集成)
    ├── Rasterizer.hpp    # 光栅化内容 (重心坐标、透视矫正、Z-Buffer)
    ├── Pipeline.hpp      # 管线调度相关 (Vertex -> Clip -> Screen)
    └── PostProcess.hpp   # 后处理 (Bloom 提取/模糊、ACES 色调映射)
