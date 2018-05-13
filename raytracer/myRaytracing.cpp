#define _CRT_SECURE_NO_WARNINGS
//#define NOMINMAX
//#include <cstdlib>
//#include <cstdio>
//#include <cmath>
//#include <fstream>
//#include <vector>
#include <iostream>
#include <ctime>
//#include <cassert>
//#include <windows.h>
//#include <string>

#include "IOPPM.h"
#include "Sphere.h"
#include "Camera.h"

//生成[0,1)的随机数
#define RAND_0_1 rand() % RAND_MAX / float(RAND_MAX)

//Fresnel equation
float fresnelMix(const float &a, const float &b, const float &mix)
{
	return b*mix + a*(1 - mix);
}

Vec3f RandomInUnitSphere()
{
	Vec3f p;
	do
	{
		p = 2.0f * Vec3f(RAND_0_1, RAND_0_1, RAND_0_1) - Vec3f(1.0f, 1.0f, 1.0f);
	} while (p.dot(p) >= 1.0f);

	return p;
}

//迟点将下面两个参数作为render函数的参数
//这里使用的环境光并不考虑遮挡效果
#define AMBIENT Vec3f(0) //Vec3f(0.3, 0.3, 0.3)		//环境光是一种trick
#define BACKGROUP_LIGHT2 Vec3f(1.5f)	//大于1的数值也是有效的
#define BACKGROUP_LIGHT1 Vec3f(0.6f, 1.1f, 1.7f)	//大于1的数值也是有效的

//raydir一定要是单位向量
//所有的光线都要相加(叠加)，但是如果有光线L与物体表面是交互的，那么光线L在叠加之前要逐分量相乘(混合)
Vec3f trace(
	const Vec3f &rayorig,
	const Vec3f &raydir,
	const std::vector<Geometry*> &geometrys,
	const int &depth,
	const int &maxDepth)
{ 
	//std::cout << depth << " ";
	float minHitDist = INFINITY;			//rayorig到最靠近的平面的距离
	const Geometry* nearSphere = NULL;		//ray击中的最靠近的球体
	for (size_t i = 0; i < geometrys.size(); ++i)
	{
		if (i >= geometrys.size() || i < 0) {
			std::cout << "vetcor下标越界" << std::endl; std::cin.get(); 
			std::cin.get();
			std::cin.get();
			std::cin.get();
			std::cin.get();
			std::cin.get();
		}
		float t = INFINITY;
		if (geometrys[i]->intersect(rayorig, raydir, t))
		{
			if (t < minHitDist)
			{
				minHitDist = t;
				nearSphere = geometrys[i];
			}
		}
	}

	//射线打不中球体，返回背景光rgb(2*255,2*255,2*255)
	if (nearSphere == NULL)
	{
#if 0
		return BACKGROUP_LIGHT;		//最简单的背景色
#else
		//渐变的背景色，之所以使用raydir，是因为我们将背景当作无限远的天空盒来处理
		Vec3f raydir_copy = raydir;
		raydir_copy.normalize();
		float t = 0.5f*(raydir_copy.y + 1.0f);
		//(1-t)*白色+t*蓝色，结果是一个蓝白的渐变
		return (1.0f - t)*BACKGROUP_LIGHT1 + t*BACKGROUP_LIGHT2;
#endif
	}
		

	//计算ray击中点的数据
	
	Vec3f phit = rayorig + raydir*minHitDist;		//ray击中点
	Vec3f nhit;
	bool inside;
	Vec2f uv;
	nearSphere->getSurfaceData(raydir, phit, nhit, inside, uv);

	//开始计算ray击中点的颜色
	float bias = 1e-4;		//偏移值

	Vec3f color = 0;
	if ((nearSphere->transparency > 0 || nearSphere->reflection > 0) && depth < maxDepth)
	{
		//下面计算反射与折射的过程都是比较简单的，因为我们只考虑一条反射线和一条折射线

		//完全镜面反射
		//只要是折射都会有镜面反射
		Vec3f refldir = raydir - nhit * 2 * raydir.dot(nhit);
		refldir.normalize();
		Vec3f reflection = trace(phit + nhit*bias, refldir, geometrys, depth + 1, maxDepth);

		//折射
		Vec3f refraction;
		if (nearSphere->transparency > 0)
		{
			//折射方向
			float ior = 1.1;														//这里统一所有的球体内部的折射率为1.1
			float niCmpT0nt = (inside) ? ior : (1 / ior);							//这个变量就是变量n，n=ni/nt
			float cosi = -nhit.dot(raydir);
			float cost = sqrt(1 - niCmpT0nt*niCmpT0nt*(1 - cosi*cosi));
			Vec3f refrdir = (niCmpT0nt*cosi - cost)*nhit + niCmpT0nt*raydir;
			refldir.normalize();

			refraction = trace(phit - nhit*bias, refrdir, geometrys, depth + 1, maxDepth);
		}

		//这个菲涅尔混合好像不太对.................
		//计算出 菲涅尔 混合值，确定折射光与反射光的叠加比例
		float facingration = -raydir.dot(nhit);
		float fresneleffect = fresnelMix(pow(1 - facingration, 3), 1, 0.1);

		color += (
				reflection*fresneleffect +									/*为什么反射效果不要乘上 nearSphere->reflection ???*/
				refraction*(1 - fresneleffect)*nearSphere->transparency
				)
				*nearSphere->getSurfaceColor(uv);
	}
	else			//表面非常粗糙时，镜面反射就近似于下面这种漫反射
	{
		color += AMBIENT * nearSphere->getSurfaceColor(uv);
#if 0
		//最简单粗暴的漫反射计算方法

		//这里我们假设：漫反射材质只从其它发光球体获取光线；这个假设明显有问题，忽略了其它光线，会导致阴影不正常
		for (unsigned i = 0; i < spheres.size(); ++i)
		{
			//排除自己，寻找发光体
			if (&spheres[i] != nearSphere && spheres[i].emissionColor.x > 0)		//isLight............
			{
				Vec3f lightDire = spheres[i].center - phit;
				lightDire.normalize();

				int j;
				for (j = 0; j < spheres.size(); ++j)
				{
					//排除当前发光体
					if (j == i)
						continue;
					//检查是否挡住光线
					float t;
					if (spheres[j].intersect(phit + nhit*bias, lightDire, t))		//isBlock..............
						break;
				}
				if (j == spheres.size())	//没物体挡住光线
				{
					color += std::max(float(0), nhit.dot(lightDire)) * spheres[i].emissionColor * nearSphere->getSurfaceColor(uv);
					//float facingration = -raydir.dot(nhit);
					//float fresneleffect = fresnelMix(pow(1 - facingration, 3), 1, 0.1);
					//color += std::max(float(0), nhit.dot(lightDire)) * trace(phit - nhit*bias, lightDire, spheres, depth + 1, maxDepth) * nearSphere->getSurfaceColor(uv) * fresneleffect;			//是不是做光照衰减才行？？？

				}
			}
		}

		//检测面积光
#else
		if (depth < maxDepth)
		{
			Vec3f target = phit + nhit + RandomInUnitSphere();
			//每次吸收50%的能量
			Vec3f diffuseDir = target - phit;
			diffuseDir.normalize();
			Vec3f diffuse = trace(phit + nhit*bias, diffuseDir, geometrys, depth + 1, maxDepth);
			color += 0.5 * diffuse * nearSphere->getSurfaceColor(uv);
		}
#endif
	}

	return color + nearSphere->emissionColor;
}

//fov是上下夹角
void render(const std::vector<Geometry*> &geometrys,
			const Vec3f &camPosition,
			const Vec3f &camDir, 
			const unsigned &pixelNumOfWidth,
			const unsigned &pixelNumOfHeight,
			const float &fov,
			const int &antialiasing = 0,
			const int &maxDepth = 5,
			const bool isGammaFix = true,
			const int maxPass = 1)
{
	//存储像素颜色，最后输入到PPM文件，所以这个数组和PPM文件是一个映射关系
	//image是所有pass结果的累加
	Vec3f *image = new Vec3f[pixelNumOfWidth*pixelNumOfHeight];
	for (int i = 0; i < pixelNumOfWidth*pixelNumOfHeight; ++i)
		image[i] = 0;
	//Vec3f *pixel = image;

	Camera *camera = new Camera(camPosition, camDir, pixelNumOfWidth, pixelNumOfHeight, fov);

	int pass = 0;
	while (pass < maxPass)
	{
		//确定每个像素的颜色，这个循环可以并行计算
		//注意：PPM文件必须按照从上到下、从左到右的顺序输入一幅图像的信息
		for (int i = pixelNumOfHeight - 1; i >= 0; --i)			//行
		{
			for (int j = 0; j < pixelNumOfWidth; ++j)			//列
			{
				Vec3f color(0);
				if (antialiasing != 0)	//或者1.....................
				{
					//ray抖动系数
					Vec2f *rayJittering = new Vec2f[antialiasing*antialiasing];		//只是当作二维数组来使用
					for (int p = 0; p < antialiasing; ++p)
						for (int q = 0; q < antialiasing; ++q)
							rayJittering[p*antialiasing + q] = Vec2f(RAND_0_1, RAND_0_1);		//为什么要减一？？？

					//面积光抖动系数

					//抖动反走样
					for (int p = 0; p < antialiasing; ++p)
					{
						for (int q = 0; q < antialiasing; ++q)
						{
							float x = j + (p + rayJittering[p*antialiasing + q].x) / antialiasing;
							float y = i + (q + rayJittering[p*antialiasing + q].y) / antialiasing;
							Vec3f primaryRayDir = camera->getCameraRayInWorld(x, y);
							primaryRayDir.normalize();
							color += trace(camPosition, primaryRayDir, geometrys, 0, maxDepth);
						}
					}
					color = color / (antialiasing*antialiasing);

					delete[] rayJittering;
				}
				else
				{
					//不带反走样
					//像素坐标转换成屏幕坐标
					float x = j + 0.5f;
					float y = i + 0.5f;

					Vec3f primaryRayDir = camera->getCameraRayInWorld(x, y);
					primaryRayDir.normalize();
					color = trace(camPosition, primaryRayDir, geometrys, 0, maxDepth);
				}

				//(*pixel++) = color;
				image[(pixelNumOfHeight - 1 - i) * pixelNumOfWidth + j] += color;
				//std::cout << "pass:" << pass + 1 << " " << "p:" << 100.0f * ((pixelNumOfHeight - 1 - i) * pixelNumOfWidth + j) / (pixelNumOfWidth * pixelNumOfHeight) << std::endl;
			}
			std::cout << "pass:" << pass + 1 << " " << "p:" << 100.0f * (pixelNumOfHeight - 1 - i) / pixelNumOfHeight << std::endl;
		}
		++pass;

		Vec3f *tempImage = new Vec3f[pixelNumOfWidth*pixelNumOfHeight];
		for (int i = 0; i < pixelNumOfWidth*pixelNumOfHeight; ++i)
		{
			tempImage[i] = image[i] / pass;
			if (isGammaFix)
			{
				//gamma修正
				//修正应该发生在pass中，还是保存图像前？？？
				tempImage[i] = Vec3f(sqrt(tempImage[i].x), sqrt(tempImage[i].y), sqrt(tempImage[i].z));			//图像变好看了，什么原理？？？
			}
		}

		out2PPM(tempImage, pixelNumOfWidth, pixelNumOfHeight, "untitled.ppm");
	} 



	delete camera;
	delete[] image;
}

inline
Vec3f mix(const Vec3f &a, const Vec3f& b, const float &mixValue)
{
	return a * (1 - mixValue) + b * mixValue;
}
Vec3f textureFunc1(const Vec2f &uv)
{
	float scale = 10;
	float pattern = (fmodf(uv.x * scale, 1) > 0.5) ^ (fmodf(uv.y * scale, 1) > 0.5);		//利用uv坐标来计算出棋盘模式，计算的结果只有0和1
	return mix(Vec3f(0, 0, 0), Vec3f(1, 1, 1), pattern);
}

Vec3f readColorFromPPM(const std::string &fileName, const Vec2f &uv)
{
	static auto width = size_t{ 0 };
	static auto height = size_t{ 0 };
	static auto maxValue = size_t{ 0 };
	static auto pixel_data = readRgbImage(fileName, &width, &height, &maxValue);

	int px = uv.x * (width - 1);
	int py = uv.y * (height - 1);

	Vec3f color;
	color.x = 1.f * (*pixel_data)[py*width * 3 + px * 3] / maxValue;
	color.y = 1.f * (*pixel_data)[py*width * 3 + px * 3 + 1] / maxValue;
	color.z = 1.f * (*pixel_data)[py*width * 3 + px * 3 + 2] / maxValue;
	return color;
}

Vec3f textureFunc2(const Vec2f &uv)
{
	return readColorFromPPM("map.ppm", uv);
}

//世界坐标：右手系
int main()
{
#if 1
	//srand(time(0));
	srand(13);
	std::vector<Geometry*> geometrys;
	//object
	geometrys.push_back(new Sphere(Vec3f(0.0, -10004, -20), 10000, Vec3f(0.8, 0.8, 0.8), NULL, 0, 0, 0.0));
	geometrys.push_back(new Sphere(Vec3f(0.0, 1, -20), 4, Vec3f(1.00, 0.32, 0.36), NULL, 0, 0.5));
	geometrys.push_back(new Sphere(Vec3f(9.0, 2.8, -17), 5, Vec3f(0.f), textureFunc2, 0));
	geometrys.push_back(new Sphere(Vec3f(-5.0, 4.0, -30), 4, Vec3f(0.36, 0.80, 0.36), NULL, 0, 0));
	//geometrys.push_back(new Sphere(Vec3f(6.0, 0, -15), 2, Vec3f(0.90, 0.76, 0.46), NULL, 1, 0.0));
	geometrys.push_back(new Sphere(Vec3f(5.0, 1, -25), 3, Vec3f(0.90, 0.76, 0.46), NULL, 1, 0.0));
	geometrys.push_back(new Sphere(Vec3f(-7.5, 1, -15), 3, Vec3f(1, 1, 1), NULL, 1, 0.0));
	geometrys.push_back(new Sphere(Vec3f(-3, -2, -11), 0.5, Vec3f(1.0), NULL, 0, 1.0));
	geometrys.push_back(new Sphere(Vec3f(4, -2.3, -13), 0.5, Vec3f(0.90,0.3,0.07), NULL));
	//light
	geometrys.push_back(new Sphere(Vec3f(0.0, 20, -30), 3, Vec3f(0.00, 0.00, 0.00), NULL, 0, 0.0, Vec3f(5)));

	//render(geometrys, Vec3f(0, 2.3, -2), Vec3f(0, 0, -1), 640, 360, 60, 0, 1, false, 1);		//5s
	//render(geometrys, Vec3f(0, 2.3, -2), Vec3f(0, 0, -1), 1280, 720, 60, 0, 1, false, 1);		//15s
	//render(geometrys, Vec3f(0, 2.3, -2), Vec3f(0, 0, -1), 1280, 720, 60, 0, 5, false, 1);		//30s
	//render(geometrys, Vec3f(0, 2.3, -2), Vec3f(0, 0, -1), 1280, 720, 90, 0, 5, false, 1);		//23s
	render(geometrys, Vec3f(0, 2.3, -2), Vec3f(0, 0, -1), 1280, 720, 60, 4, 5, false, 1);		//7min46s
	//render(geometrys, Vec3f(0, 2.3, -2), Vec3f(0, 0, -1), 1280, 720, 60, 4, 5, false, 3);		//22min10s
	//render(geometrys, Vec3f(0, 2.3, -2), Vec3f(0, 0, -1), 1280, 720, 60, 4, 5, false, 3);		//29min50s
	//render(geometrys, Vec3f(0, 2.3, -2), Vec3f(0, 0, -1), 1280, 720, 60, 4, 5, true, 3);		//30min30s

#elif 0
	auto width = size_t{ 0 };
	auto height = size_t{ 0 };
	auto pixel_data = std::vector<uint8_t>();
	readRgbImage("untitled.ppm", &width, &height, &pixel_data);

	Vec3f *colors = new Vec3f[width * height];
	unsigned char value;
	for (int i = 0; i < width * height * 3; ++i)
	{
		float color = 1.f * pixel_data[i] / 255;
		if (i % 3 == 0)
			colors[i / 3].x = color;
		else if (i % 3 == 1)
			colors[i / 3].y = color;
		else
			colors[i / 3].z = color;
	}

	out2PPM(colors, width, height, "copy.ppm");

	delete[] colors;

#else
	std::cout << RAND_MAX;
	std::cin.get();
#endif

	return 0;
}