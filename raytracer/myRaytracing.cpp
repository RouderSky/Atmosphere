#if 1

//#include <cstdlib>
//#include <cstdio>
//#include <cmath>
#include <fstream>
#include <vector>
#include <iostream>
//#include <cassert>

#include "Sphere.h"

#define MAX_RAY_DEPTH 5		//递归深度

//Fresnel equation
float fresnelMix(const float &a, const float &b, const float &mix)
{
	return b*mix + a*(1 - mix);
}

//任务：
//实现2D纹理映射
//理解球体uv坐标的计算
//使用更高级的漫反射计算方法
//加入多边形网格，为其加上纹理
//

//这里使用的环境光并不考虑遮挡效果
#define AMBIENT Vec3f(0.05, 0.05, 0.05)
#define BACKGROUP_COLOR Vec3f(2)
//raydir一定要是单位向量
//所有的光线都要相加(叠加)，但是如果有光线L与物体表面是交互的，那么光线L在叠加之前要逐分量相乘(混合)
Vec3f trace(
	const Vec3f &rayorig,
	const Vec3f &raydir,
	const std::vector<Sphere> &spheres,
	const int &depth)
{
	float minHitDist = INFINITY;			//rayorig到最靠近的平面的距离
	const Sphere* nearSphere = NULL;		//ray击中的最靠近的球体
	for (unsigned i = 0; i < spheres.size(); ++i)
	{
		float t = INFINITY;
		if (spheres[i].intersect(rayorig, raydir, t))
		{
			if (t < minHitDist)
			{
				minHitDist = t;
				nearSphere = &spheres[i];
			}
		}
	}

	if (nearSphere == NULL)
		return BACKGROUP_COLOR + AMBIENT;							//射线打不中球体，返回背景色rgb(2*255,2*255,2*255)

	//计算ray击中点的数据
	Vec3f color = 0;
	Vec3f phit = rayorig + raydir*minHitDist;		//ray击中点

	Vec3f nhit;
	bool inside;
	Vec2f uv;
	nearSphere->getSurfaceData(raydir, phit, nhit, inside, uv);

	//开始计算ray击中点的颜色
	float bias = 1e-4;		//偏移值

	//难道 漫反射 和 镜面反射+折射 是互斥的吗？不应该是互斥的，将下面的else去掉就好
	if ((nearSphere->transparency > 0 || nearSphere->reflection > 0) && depth < MAX_RAY_DEPTH)
	{
		//下面计算反射与折射的过程都是比较简单的，因为我们只考虑一条反射线和一条折射线

		//计算镜面反射效果，不判断就直接计算，是因为只要是折射都会有镜面反射吗？？？？
		Vec3f refldir = raydir - nhit * 2 * raydir.dot(nhit);
		//refldir.normalize();
		//递归
		Vec3f reflection = trace(phit + nhit*bias, refldir, spheres, depth + 1);	//这里让shadow ray的发射点产生偏差

		//计算折射效果
		Vec3f refraction;
		if (nearSphere->transparency > 0)
		{
			//计算折射方向
			float ior = 1.1;														//这里统一所有的球体内部的折射率为1.1
			float niCmpT0nt = (inside) ? ior : (1 / ior);							//这个变量就是变量n，n=ni/nt
			float cosi = -nhit.dot(raydir);
			float cost = sqrt(1 - niCmpT0nt*niCmpT0nt*(1 - cosi*cosi));
			Vec3f refrdir = (niCmpT0nt*cosi - cost)*nhit + niCmpT0nt*raydir;
			//refldir.normalize();
			//递归
			refraction = trace(phit - nhit*bias, refrdir, spheres, depth + 1);		//这里让shadow ray的发射点产生偏差
		}

		//计算出 菲涅尔 混合值，确定折射光与反射光的叠加比例
		float facingration = -raydir.dot(nhit);
		float fresneleffect = fresnelMix(pow(1 - facingration, 3), 1, 0.1);

		color = (
				reflection*fresneleffect +									/*为什么反射效果不要乘上 nearSphere->reflection ???*/
				refraction*(1 - fresneleffect)*nearSphere->transparency + 
				AMBIENT
				)
				*nearSphere->getSurfaceColor(uv);
	}
	else
	{
		//为什么漫反射不用递归？？？
		//这里我们假设：漫反射材质只从其它发光球体获取光线；这个假设明显有问题，会导致阴影不正常
		color += AMBIENT * nearSphere->getSurfaceColor(uv);
		for (unsigned i = 0; i < spheres.size(); ++i)
		{
			if (&spheres[i]!=nearSphere && spheres[i].emissionColor.x > 0)			//排除自己，寻找发光体			
			{
				//检查有没有挡住光线
				bool block = false;
				Vec3f lightDire = spheres[i].center - phit;
				lightDire.normalize();
				int j;
				for (j = 0; j < spheres.size(); ++j)
				{
					if(j==i)		//排除发光体
						continue;
					float t;
					if (spheres[j].intersect(phit + nhit*bias, lightDire, t))
						break;
				}
				if (j == spheres.size())	//没物体挡住光线
					color += std::max(float(0), nhit.dot(lightDire)) * spheres[i].emissionColor * nearSphere->getSurfaceColor(uv);
			}
		}
	}

	return color + nearSphere->emissionColor;
}

inline 
float clamp(const float &lo, const float &hi, const float &v)
{ 
	return std::max(lo, std::min(hi, v)); 
}

//fov是上下夹角
void render(const std::vector<Sphere> &spheres,const unsigned &pixelNumOfWidth,const unsigned &pixelNumOfHeight,const float &fov)
{
	//存储像素颜色，最后输入到PPM文件，所以这个数组和PPM文件是一个映射关系
	Vec3f *image = new Vec3f[pixelNumOfWidth*pixelNumOfHeight];
	Vec3f *pixel = image;

	//确定每一个像素的位置，然后确定primary ray的方向
	//这里我们假定：摄像机坐标系与世界坐标系完全重合，所以我们不需要坐标系转换矩阵；且cavas这个平面位于z=-1这个平面上
	float aspectratio = pixelNumOfWidth / (float)pixelNumOfHeight;
	float height = 2 * tan(fov / 2 * M_PI / 180);		//canvas宽度
	float width = height * aspectratio;					//canvas高度
	float widthOfPixel = width / pixelNumOfWidth;		//canvas上的像素宽度
	float heightOfPixel = height / pixelNumOfHeight;	//canvas上的像素高度
	//初始坐标
	float x0 = -(width / 2) + (widthOfPixel / 2);
	float y0 = (height / 2) - (heightOfPixel / 2);
	//确定每个像素的颜色，这个循环可以并行计算
	//注意：PPM文件必须按照从上到下、从左到右的顺序输入一幅图像的信息
	for (int i = 0; i < pixelNumOfHeight; ++i)			//行
	{
		for (int j = 0; j < pixelNumOfWidth; ++j)		//列
		{
			float pixelX = x0 + j*widthOfPixel;
			float pixelY = y0 - i*heightOfPixel;
			Vec3f primaryRayDir(pixelX, pixelY, -1);
			primaryRayDir.normalize();
			(*pixel++) = trace(Vec3f(0), primaryRayDir, spheres, 0);	//第一个参数可以是像素点的位置
		}
	}

	//输出到PPM文件
	std::ofstream ofs("./untitled.ppm", std::ios::out | std::ios::binary);
	ofs << "P6\n" << pixelNumOfWidth << " " << pixelNumOfHeight << "\n255\n";
	for (unsigned i = 0; i < pixelNumOfWidth*pixelNumOfHeight; ++i)
	{
		//四色五入
		ofs << (unsigned char)(clamp(0.f, 1.f, image[i].x) * 255 + 0.5)
			<< (unsigned char)(clamp(0.f, 1.f, image[i].y) * 255 + 0.5)
			<< (unsigned char)(clamp(0.f, 1.f, image[i].z) * 255 + 0.5);
	}
	ofs.close();
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

Vec3f texttureFunc2(const Vec2f &uv)
{
	return NULL;
}

int main()
{
	srand(13);	//干什么的？？？
	std::vector<Sphere> spheres;
	//object
	spheres.push_back(Sphere(Vec3f(0.0, -10004, -20), 10000, Vec3f(0.8, 0.5, 0.15), NULL, 0, 0, 0.0));
	spheres.push_back(Sphere(Vec3f(0.0, 0, -20), 4, Vec3f(1.00, 0.32, 0.36), NULL, 1, 0.5));
	spheres.push_back(Sphere(Vec3f(-5.0, 3.0, -30), 4, Vec3f(0.36, 0.80, 0.36), NULL, 0, 0));
	spheres.push_back(Sphere(Vec3f(6.0, -1, -15), 2, Vec3f(0.90, 0.76, 0.46), NULL, 1, 0.0));
	spheres.push_back(Sphere(Vec3f(5.0, 0, -25), 3, Vec3f(0.65, 0.77, 0.97), NULL, 1, 0.0));
	spheres.push_back(Sphere(Vec3f(-7.5, 0, -15), 3, Vec3f(0.90, 0.90, 0.90), NULL, 1, 0.0));
	//light
	spheres.push_back(Sphere(Vec3f(0.0, 20, -30), 3, Vec3f(0.00, 0.00, 0.00), NULL, 0, 0.0, Vec3f(3)));
	render(spheres, 1280, 720, 60);
	return 0;
}

#endif