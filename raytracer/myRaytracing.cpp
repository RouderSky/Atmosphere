#include <cstdlib>
#include <cstdio>
#include <cmath>
#include <fstream>
#include <vector>
#include <iostream>
#include <cassert>
#include <algorithm>

#if defined __linux__ || defined __APPLE__
//For Linux
#else
//For Windows
#define M_PI 3.141592653589793 
#endif

template<typename T>
class Vec3
{
public:
	T x, y, z;
	Vec3() :x(0), y(0), z(0) {}
	Vec3(T xx) :x(xx), y(xx), z(xx) {}
	Vec3(T xx, T yy, T zz) :x(xx), y(yy), z(zz) {}
	Vec3& normalize()
	{
		T len = length();
		if (len > 0)
		{
			T fac = 1 / len;
			x *= fac;
			y *= fac;
			z *= fac;
		}
		return *this;
	}
	Vec3<T> operator * (const T &f) const { return Vec3<T>(x*f, y*f, z*f); }
	friend Vec3<T> operator * (const T &f, const Vec3<T> &v) { return v*f; }
	Vec3<T> operator * (const Vec3<T> &v) const { return Vec3<T>(x*v.x, y*v.y, z*v.z); }	//逐分量相乘
	T dot(const Vec3<T> &v) const { return x*v.x + y*v.y + z*v.z; }
	Vec3<T> operator - (const Vec3<T> &v) const { return Vec3<T>(x - v.x, y - v.y, z - v.z); }
	Vec3<T> operator + (const Vec3<T> &v) const { return Vec3<T>(x + v.x, y + v.y, z + v.z); }
	Vec3<T>& operator +=(const Vec3<T> &v) { x += v.x, y += v.y, z += v.z; return *this; }
	Vec3<T>& operator -=(const Vec3<T> &v) { x -= v.x, y -= v.y, z -= v.z; return *this; }
	Vec3<T> operator - () const { return Vec3<T>(-x, -y, -z); }
	T length2() const { return x*x + y*y + z*z; }
	T length() const { return sqrt(length2()); }
	friend std::ostream & operator << (std::ostream &os, const Vec3<T> &c)
	{
		os << "[" << v.x << " " << v.y << " " << v.z << "]";
		return os;
	}
};

typedef Vec3<float> Vec3f;

class Sphere
{
public:
	Vec3f center;
	float radius, radius2;
	Vec3f surfaceColor, emissionColor;		//自发光颜色
	float transparency, reflection;			//还可以加上一个折射率属性
	Sphere(
		const Vec3f &c,
		const float &r,
		const Vec3f &sc,
		const float &refl = 0,
		const float &transp = 0,
		const Vec3f &ec = 0) :
		center(c), radius(r), radius2(r*r), surfaceColor(sc), emissionColor(ec), transparency(transp), reflection(refl)
	{}

	//计算点center到直线raydir的距离d，如果d小于等于radius，那么就相交(rayorig可能在一个圆的内部)
	//t0和t1用于接收相交点到oig的距离：如果ray的发射点在球体内部，t1是击中距离；如果ray的发射点在球体外部，t0是击中距离
	bool intersect(const Vec3f &rayorig, const Vec3f &raydir, float &t0, float &t1)const	//这里设置两个float似乎是多余的,迟点改写这个函数。。。。。。
	{
		Vec3f l = center - rayorig;
		float projectOflToRaydir = l.dot(raydir);
		if (projectOflToRaydir < 0)
			return false;
		float d2 = l.length2() - projectOflToRaydir*projectOflToRaydir;
		if (d2 > radius2)
			return false;
		float offset = sqrt(radius - d2);
		t0 = projectOflToRaydir - offset;
		t1 = projectOflToRaydir + offset;
	}
};

#define MAX_RAY_DEPTH 5		//递归深度

//这是什么混合方法
float mix(const float &a, const float &b, const float &mix)
{
	return b*mix + a*(1 - mix);
}

//raydir一定要是单位向量
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
		float t0 = INFINITY, t1 = INFINITY;
		if (spheres[i].intersect(rayorig, raydir, t0, t1))
		{
			int hitFaceDist = t0 < 0/*rayorig在物体内部*/ ? t1 : t0;
			if (hitFaceDist < minHitDist)
			{
				minHitDist = hitFaceDist;
				nearSphere = &spheres[i];
			}
		}
	}

	if (nearSphere == NULL)
		return Vec3f(2);		//射线打不中球体，返回背景色rgb(2*255,2*255,2*255)

	//计算ray击中点的数据
	Vec3f surfaceColor = 0;
	Vec3f phit = rayorig + raydir*minHitDist;		//ray击中点
	Vec3f nhit = phit - nearSphere->center;			//ray击中点的球体外法线
	nhit.normalize();
	bool inside = false;	//ray的发射点是不是在内部
	if (raydir.dot(nhit))
	{
		nhit = -nhit;
		inside = true;
	}

	//开始计算ray击中点的颜色
	float bias = 1e-4;		//误差值
	//难道 漫反射 和 镜面反射+折射 冲突吗？？？？
	if ((nearSphere->transparency > 0 || nearSphere->reflection > 0) && depth < MAX_RAY_DEPTH)
	{
		//计算镜面反射效果，不判断就直接计算，是因为只要是折射都会有反射吗？？？？
		Vec3f refldir = raydir - nhit * 2 * raydir.dot(nhit);	//反射方向
		refldir.normalize();	//似乎没有必要归一化，因为入射向量是单位向量
		//递归
		Vec3f reflection = trace(phit + nhit*bias, refldir, spheres, depth + 1);	//这里让shadow ray的发射点产生偏差，如果不做偏差会怎么样？。。。。。。

		//计算折射效果
		Vec3f refraction;
		if (nearSphere->transparency > 0)
		{
			//计算折射方向
			float ior = 1.1;	//这里统一所有的球体内部的折射率为1.1
			float niCmpT0nt = (inside) ? ior : (1 / ior);	//这个变量就是变量n，n=ni/nt
			float cosi = -nhit.dot(raydir);
			float cost = sqrt(1 - niCmpT0nt*niCmpT0nt*(1 - cosi*cosi));
			Vec3f refrdir = (niCmpT0nt*cosi - cost)*nhit + niCmpT0nt*raydir;
			refldir.normalize();
			//递归
			refraction = trace(phit - nhit*bias, refrdir, spheres, depth + 1);		//这里让shadow ray的发射点产生偏差，但是为什么这里是减，上面是加？？？
		}

		//计算出 菲涅尔 混合值
		float facingration = -raydir.dot(nhit);
		float fresneleffect = mix(pow(1 - facingration, 3), 1, 0.1);
		//怎么混合的？？？
		surfaceColor = (reflection*fresneleffect +						/*为什么反射效果不要乘上 nearSphere->reflection ???*/
			refraction*(1 - fresneleffect)*nearSphere->transparency)
			*nearSphere->surfaceColor;
			
	}
	else
	{
		//为什么漫反射不用递归？？？
		for (unsigned i = 0; i < spheres.size(); ++i)
		{
			//这里应该要将自己排除在外，因为自己不能接收自己的光线
			if (spheres[i].emissionColor.x > 0)			//这里我们假设：漫反射材质只从其它发光球体获取光线；这个假设明显有问题
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
					float t0, t1;
					if (spheres[j].intersect(phit + nhit*bias, lightDire, t0, t1))
						break;
				}
				if (j == spheres.size())	//没物体挡住光线
					surfaceColor += std::max(float(0), nhit.dot(lightDire))*spheres[i].emissionColor*nearSphere->surfaceColor;		//怎么混合的？？
			}
		}
	}

	return surfaceColor + nearSphere->emissionColor;
}

//void render(const std::vector<Sphere> &spheres,const unsigned &pixelNumOfWidth,const unsigned &pixelNumOfHeight,const float &fov)
//{
//	//存储像素颜色，最后输入到PPM文件，所以这个数组和PPM文件是一个映射关系
//	Vec3f *image = new Vec3f[pixelNumOfWidth*pixelNumOfHeight];
//	Vec3f *pixel = image;
//
//	//确定每一个像素的位置，然后确定primary ray的方向
//	//这里我们假定cavas这个平面位于z=-1这个平面上
//	float aspectratio = pixelNumOfWidth / (float)pixelNumOfHeight;
//	float width = 2 * tan(fov / 2 * M_PI / 180);		//canvas宽度
//	float height = width / aspectratio;					//canvas高度
//	float widthOfPixel = width / pixelNumOfWidth;		//canvas上的像素宽度
//	float heightOfPixel = height / pixelNumOfHeight;	//canvas上的像素高度
//	//
//	float x0 = -(width / 2) + (widthOfPixel / 2);
//	float y0 = (height / 2) - (heightOfPixel / 2);
//	//确定每个像素的颜色，这个循环可以并行计算
//	//注意：PPM文件必须按照从上到下、从左到右的顺序输入一幅图像的信息
//	for (int i = 0; i < pixelNumOfHeight; ++i)			//行
//	{
//		for (int j = 0; j < pixelNumOfWidth; ++j)		//列
//		{
//			float pixelX = x0 + j*widthOfPixel;
//			float pixelY = y0 - i*heightOfPixel;
//			Vec3f primaryRayDir(pixelX, pixelY, -1);
//			primaryRayDir.normalize();
//			(*pixel++) = trace(Vec3f(0), primaryRayDir, spheres, 0);	//第一个参数可以是像素点的位置
//		}
//	}
//
//	//输出到PPM文件
//	std::ofstream ofs("./untitled.ppm", std::ios::out | std::ios::binary);
//	ofs << "P6\n" << pixelNumOfWidth << " " << pixelNumOfHeight << "\n255\n";
//	for (unsigned i = 0; i < pixelNumOfWidth*pixelNumOfHeight; ++i)
//	{
//		ofs << (unsigned char)(std::min((float)1, image[i].x) * 255)
//			<< (unsigned char)(std::min((float)1, image[i].y) * 255)
//			<< (unsigned char)(std::min((float)1, image[i].z) * 255);	//不做四舍五入了？？？
//	}
//	ofs.close();
//	delete[] image;
//}

void render(const std::vector<Sphere> &spheres, const unsigned &pixelNumOfWidth, const unsigned &pixelNumOfHeight, const float &fov)
{
	//存储像素颜色，最后输入到PPM文件，所以这个数组和PPM文件是一个映射关系
	Vec3f *image = new Vec3f[pixelNumOfWidth*pixelNumOfHeight];
	Vec3f *pixel = image;

	//确定每一个像素的位置，然后确定primary ray的方向
	//这里我们假定cavas这个平面位于z=-1这个平面上
	float aspectratio = pixelNumOfWidth / (float)pixelNumOfHeight;
	float width = 2 * tan(fov / 2 * M_PI / 180);		//canvas宽度
	float height = width / aspectratio;					//canvas高度
	float widthOfPixel = width / pixelNumOfWidth;		//canvas上的像素宽度
	float heightOfPixel = height / pixelNumOfHeight;	//canvas上的像素高度
														//
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
		ofs << (unsigned char)(std::min((float)1, image[i].x) * 255)
			<< (unsigned char)(std::min((float)1, image[i].y) * 255)
			<< (unsigned char)(std::min((float)1, image[i].z) * 255);	//不做四舍五入了？？？
	}
	ofs.close();
	delete[] image;
}

int main()
{
	srand(13);
	std::vector<Sphere> spheres;
	//object
	spheres.push_back(Sphere(Vec3f(0.0, -10004, -20), 10000, Vec3f(0.20, 0.20, 0.20), 0, 0, 0.0));
	spheres.push_back(Sphere(Vec3f(0.0, 0, -20), 4, Vec3f(1.00, 0.32, 0.36), 1, 0.5));
	spheres.push_back(Sphere(Vec3f(5.0, -1, -15), 2, Vec3f(0.90, 0.76, 0.46), 1, 0.0));
	spheres.push_back(Sphere(Vec3f(5.0, 0, -25), 3, Vec3f(0.65, 0.77, 0.97), 1, 0.0));
	spheres.push_back(Sphere(Vec3f(-5.5, 0, -15), 3, Vec3f(0.90, 0.90, 0.90), 1, 0.0));
	//light
	spheres.push_back(Sphere(Vec3f(0.0, 20, -30), 3, Vec3f(0.00, 0.00, 0.00), 0, 0.0, Vec3f(3)));
	render(spheres, 640, 480, 30);
	//render(spheres);
	return 0;
}