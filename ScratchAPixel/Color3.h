#ifndef __COLOR3_H__
#define __COLOR3_H__

#include <fstream>
#include <algorithm>

template<typename T>
class Color3
{
public:
	Color3(): r(0), g(0), b(0){}
	Color3(T rr) : r(rr), g(rr), b(rr) {}
	Color3(T rr, T gg, T bb) :r(rr), g(gg), b(bb) {}
	Color3 operator * (const T &v) const
	{
		return Color3(r*v, g*v, b*v);
	}
	friend Color3 operator * (const T &v, const Color3 &c)		//scratchapixel将T直接设置为float，估计是它写错了
	{
		return c*v;
	}
	Color3 operator + (const Color3 &c) const
	{
		return Color3(r + c.r, g + c.g, b + c.g);
	}
	friend std::ostream & operator << (std::ostream &os, const Color3 &c)
	{
		os << c.r << " " << c.g << " " << c.g;
		return os;
	}

	T r, g, b;		//scratchapixel将T直接设置为float，估计是它写错了
};

typedef Color3<float> Color3f;

//将像素输出到PPM文件中，PPM文件可以用photoshop打开
//fn为PPM文件名
//c存储在所有像素的颜色
//width为图像宽度，单位为像素
//height为图像高度，单位为像素
void saveToPPM(const char *fn, const Color3f *c, const int &width, const int &height)
{
	std::ofstream ofs;
	//由于我在windows平台，flag要写上，用来设置格式的，但是我不知道怎么写。。。。。。。。。。
	ofs.open(fn, std::ios::out | std::ios::binary);

	if (ofs.fail())
	{
		fprintf(stderr, "ERROR: cant;t save image to file %s\n", fn);	//stderr是什么文件？
	}
	else
	{
		ofs << "P6\n" << width << " " << height << "\n255\n";
		const Color3f *pc = c;
		for (int i = 0; i < height; ++i)
		{
			for (int j = 0; j < width; ++j)
			{
				char r = static_cast<char>(std::min(255.f, 255 * pc->r + 0.5f));		//为什么使用static_cast？
				char g = static_cast<char>(std::min(255.f, 255 * pc->g + 0.5f));
				char b = static_cast<char>(std::min(255.f, 255 * pc->b + 0.5f));
				ofs << r << g << b;
				++pc;
			}
		}
	}

	ofs.close();
}



#endif
