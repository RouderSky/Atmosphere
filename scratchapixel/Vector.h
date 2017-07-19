#ifndef __VECTOR_H__
#define __VECTOR_H__

#include <cstdlib> 
#include <cstdio> 
#include <iostream> 
#include <iomanip> 

template<typename T>
class Vec3
{
public:
	T x, y, z;

	Vec3() : x(T(0)), y(T(0)), z(T(0)){}
	Vec3(const T &xx) : x(xx), y(xx), z(xx){}		//调用时即使参数类型与T不匹配也可以，访问xx会自动强转
	Vec3(const T &xx, const T &yy, const T &zz) :x(xx), y(yy), z(zz){}
	
	Vec3<T> operator + (const Vec3<T> &v) const
	{
		return Vec3<T>(x + v.x, y + v.y, z + v.z);
	}

	Vec3<T> operator - (const Vec3<T> &v) const
	{
		return Vec3<T>(x - v.x, y - v.y, z - v.z);
	}

	//这个写法仅支持向量乘系数，而不支持系数乘矩阵
	Vec3<T> operator * (const T &r) const
	{
		return Vec3<T>(x * r, y * r, z * r);
	}

	//右边的const代表着该函数不会修改该对象中的任何成员变量，一旦在函数中不小心改了成员变量，编译无法通过，增加了函数安全性
	//如果该对象在外面被初始化为const类型，那么这个对象是不可以调用非const成员函数的,只能调用const成员函数；给函数加上const声明之后，函数的可用性提高了
	T dotProduct(const Vec3<T> &v) const
	{
		return x*v.x + y*v.y + z*v.z;
	}

	Vec3<T> crossProduct(const Vec3<T> &v) const
	{
		return Vec3<T>(
			y*v.z - z*v.y,
			z*v.x - x*v.z,
			x*v.y - y*v.x
			);
	}

	//这个函数名字是什么意思？？？
	T norm() const
	{
		return x*x + y*y + z*z;
	}

	T length()
	{
		return sqrt(norm());				//这个计算方法不是最快的
	}

	//返回的是引用，而且提供了两个版本
	const T& operator [] (uint8_t i) const	//这个版本有什么意义？？？下面都有一个版本是可以更改数据的了
	{
		return (&x)[i];		//还有这种操作的？？？x，y，z一定是连续空间的吗？？？？
	}
	T& operator [] (uint8_t i)
	{
		return (&x)[i];		//还有这种操作的？？？x，y，z一定是连续空间的吗？？？？
	}

	Vec3<T>& normalize()							//返回引用
	{
		T len = length();
		if (len > 0)
		{
			T invLen = 1 / len;						//除法耗费的性能比乘法要大，所以先处理一下
			x *= invLen, y *= invLen, z *= invLen;	
		}
		return *this;								//这个return多此一举，因为如果外部可以调用这个函数，证明了外面已经拿到了这个向量的本体了，返回完全是多余
	}

	friend std::ostream& operator << (std::ostream &s, const Vec3<T> &v)
	{
		return s << '(' << v.x << ' ' << v.y << ' ' << v.z << ')';
	}

};

typedef Vec3<float> Vec3f;
typedef Vec3<int> Vec3i;

//----------------------------以下代码用到的机会不多

//球面坐标到笛卡尔坐标
template<typename T>
Vec3<T> sphericalToCartesian(const T &theta, const T &phi)
{
	return Vec3<T>(cos(phi)*sin(theta), sin(phi)*sin(theta), cos(theta));
}

//笛卡尔坐标到球面坐标
//输入的向量一定要是一个单位向量
template<typename T>
inline T sphericalTheta(const Vec3<T> &v)
{
	return acos(clamp<T>(v[2], -1, 1));		//为了安全考虑，避免v[2]数据存在微小的误差
}

//笛卡尔坐标到球面坐标
template<typename T>
inline T sphericalPhi(const Vec3<T> &v)
{
	T p = atan2(v[1], v[0]);
	return (p < 0) ? p + 2 * M_PI : p;		//由于角度一旦大于180度，atan会自动返回对应负角度；所以，为了避免混乱，这里手动统一，无论怎么样，都会返回一个正的角度；
}

//球面坐标θ的cos值
//输入的向量一定要是一个单位向量
template<typename T>
inline T cosTheta(const Vec3<T> &w)
{
	return w[2];
}

//球面坐标θ的sin值的平方
//输入的向量一定要是一个单位向量
template<typename T>
inline T sinTheta2(const Vec3<T> &w)
{
	return std::max(T(0), 1 - cosTheta(w)*cosTheta(w));		//1-c的平方有可能得到负数？
}

//球面坐标θ的sin值
//输入的向量一定要是一个单位向量
template<typename T>
inline T sinTheta(const Vec3<T> &w)
{
	return sqrt(sinTheta2(w));
}

//球面坐标Phi的sin值
//输入的向量一定要是一个单位向量
template<typename T>
inline T cosPhi(const Vec3<T> &w)
{
	T sintheta = sinTheta(w);
	if (sintheta == 0) return 1;
	return clamp<T>(w[0] / sintheta, -1, 1);
}

//球面坐标Phi的cos值
//输入的向量一定要是一个单位向量
template<typename T>
inline T sinPhi(const Vec3<T> &w)
{
	T sintheta = sinTheta(w);
	if (sintheta == 0) return 0;
	return clamp<T>(w[1] / sintheta, -1, 1);
}
#endif