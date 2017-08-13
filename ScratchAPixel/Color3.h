#ifndef __COLOR3_H__
#define __COLOR3_H__

#include <ostream>

template<typename T>
class Color3
{
public:
	Color3 : r(0), g(0), b(0) {}
	Color3(T rr) : r(rr), g(rr), b(rr) {}
	Color3(T rr, T gg, T bb) :r(rr), g(gg), b(bb) {}
	Color3 operator * (const T &v) const
	{
		return Color3(r*v, g*v, b*v);
	}
	friend Color3 operator * (const T &v, const Color3 &c)		//scratchapixel��Tֱ������Ϊfloat����������д����
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

	T r, g, b;		//scratchapixel��Tֱ������Ϊfloat����������д����
};

typedef Color<float> Color3f;

#endif
