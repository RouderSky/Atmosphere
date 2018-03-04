<h1 align="center">Render</h1>

ps:该项目创建于vs2015，请使用支持c++11的编译器编译

## scratchapixel ##
### 进度 ###
- class Vector
- class Matrix
- class Color3
- class Interpolation
- class Camera

## ray-tracing ##
> Whitted-style ray-tracing
### 进度 ###
- **Specular reflection** 和 **Refraction**：利用 **Fresnel equation** 
- **Diffuse reflection** ：没有考虑 **Sky light**，仅考虑 **Spot light**，且没有进行 **recursion**
- **Texture Mapping for sphere**：利用球的极坐标计算uv坐标
- 加入摄像机空间
### 待完成 ###
- 实现反走样
- 实现软阴影
- 实现光泽反射
- 加入模型空间
- 实现更高级的漫反射
- 加入polygon mesh
- 实现motion blur

### 缺陷 ###
- 阴影不正常，不应该为全黑

### 效果 ###
![](untitled.jpg)
