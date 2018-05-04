#ifndef _GEOMETRY_H__
#define _GEOMETRY_H__

//作废

/*

#include <algorithm>

#if defined __linux__ || defined __APPLE__
//For Linux
#else
//For Windows
#define M_PI 3.141592653589793 
#endif

template<typename T>
class Vec2
{
public:
	Vec2() : x(0), y(0) {}
	Vec2(T xx) : x(xx), y(xx) {}
	Vec2(T xx, T yy) : x(xx), y(yy) {}
	Vec2 operator + (const Vec2 &v) const
	{
		return Vec2(x + v.x, y + v.y);
	}
	Vec2 operator / (const T &r) const
	{
		return Vec2(x / r, y / r);
	}
	Vec2 operator * (const T &r) const
	{
		return Vec2(x * r, y * r);
	}
	Vec2& operator /= (const T &r)
	{
		x /= r, y /= r; return *this;
	}
	Vec2& operator *= (const T &r)
	{
		x *= r, y *= r; return *this;
	}
	friend std::ostream& operator << (std::ostream &s, const Vec2<T> &v)
	{
		return s << '[' << v.x << ' ' << v.y << ']';
	}
	friend Vec2 operator * (const T &r, const Vec2<T> &v)
	{
		return Vec2(v.x * r, v.y * r);
	}
	T x, y;
};

typedef Vec2<float> Vec2f;
typedef Vec2<int> Vec2i;

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
	Vec3<T> operator * (const Vec3<T> &v) const { return Vec3<T>(x*v.x, y*v.y, z*v.z); }
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

*/

#endif