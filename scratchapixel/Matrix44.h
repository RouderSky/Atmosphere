#ifndef __MATRIX44_H__
#define __MATRIX44_H__

#include <cstdlib> 
#include <cstdio> 
#include <iostream> 
#include <iomanip> 

template<typename T>
class Matrix44
{
public:
	//将矩阵默认初始化为单位矩阵
	T m[4][4] = { { 1, 0, 0, 0 }, { 0, 1, 0, 0 }, { 0, 0, 1, 0 }, { 0, 0, 0, 1 } };

	Matrix44(){}

	Matrix44(T a, T b, T c, T d, T e, T f, T g, T h, T i, T j, T k, T l, T m, T n, T o, T p)
	{
		m[0][0] = a;
		m[0][1] = b;
		m[0][2] = c;
		m[0][3] = d;
		m[1][0] = e;
		m[1][1] = f;
		m[1][2] = g;
		m[1][3] = h;
		m[2][0] = i;
		m[2][1] = j;
		m[2][2] = k;
		m[2][3] = l;
		m[3][0] = m;
		m[3][1] = n;
		m[3][2] = o;
		m[3][3] = p;
	}

	//重载访问操作符
	//注意：即使函数的参数列表相同，返回值加上const也算是一种重载
	//uint8_t是什么数据类型？？？？？？？？？？？？？？
	const T* operator [] (uint8_t i) const{ return m[i]; }		//传出的数组不可改变,这个版本有实际意义吗？？？
	T* operator [] (uint8_t i) const{ return m[i]; }			//传出的数组可以改变

	//这个函数就奇怪了，为什么Matrix44不用声明类型T了？？？？？
	Matrix44 operator * (const Matrix44& v) const
	{
		Matrix44 tmp;
		multiply(*this, v, tmp);
		return tmp;
	}

	//这里的函数也没有<T>？？？，而且参数列表有时候有<T>,有时候又没有<T>？？？
	static void multiply(const Matrix44<T> &a, const Matrix44& b, Matrix44 &c)
	{
#if 0
		//最容易理解的写法
		for (uint8_t i=0;i<4;++i)
		{
			for (uint8_t j = 0; j < 4; ++j)
			{
				c[i][j] = a[i][0] * b[0][j] + a[i][1] * b[1][j] + a[i][2] * b[2][j] + a[i][3] * b[3][j];
			}
		}
#else
		//这个写法和上面的功能是一样的
		//__restrict是什么东西？？？
		//性能比较好，但是看不懂写的是什么.........
		const T * __restrict ap = &a.x[0][0];
		const T * __restrict bp = &b.x[0][0];
		T * __restrict cp = &c.x[0][0];

		T a0, a1, a2, a3;

		a0 = ap[0];
		a1 = ap[1];
		a2 = ap[2];
		a3 = ap[3];

		cp[0] = a0 * bp[0] + a1 * bp[4] + a2 * bp[8] + a3 * bp[12];
		cp[1] = a0 * bp[1] + a1 * bp[5] + a2 * bp[9] + a3 * bp[13];
		cp[2] = a0 * bp[2] + a1 * bp[6] + a2 * bp[10] + a3 * bp[14];
		cp[3] = a0 * bp[3] + a1 * bp[7] + a2 * bp[11] + a3 * bp[15];

		a0 = ap[4];
		a1 = ap[5];
		a2 = ap[6];
		a3 = ap[7];

		cp[4] = a0 * bp[0] + a1 * bp[4] + a2 * bp[8] + a3 * bp[12];
		cp[5] = a0 * bp[1] + a1 * bp[5] + a2 * bp[9] + a3 * bp[13];
		cp[6] = a0 * bp[2] + a1 * bp[6] + a2 * bp[10] + a3 * bp[14];
		cp[7] = a0 * bp[3] + a1 * bp[7] + a2 * bp[11] + a3 * bp[15];

		a0 = ap[8];
		a1 = ap[9];
		a2 = ap[10];
		a3 = ap[11];

		cp[8] = a0 * bp[0] + a1 * bp[4] + a2 * bp[8] + a3 * bp[12];
		cp[9] = a0 * bp[1] + a1 * bp[5] + a2 * bp[9] + a3 * bp[13];
		cp[10] = a0 * bp[2] + a1 * bp[6] + a2 * bp[10] + a3 * bp[14];
		cp[11] = a0 * bp[3] + a1 * bp[7] + a2 * bp[11] + a3 * bp[15];

		a0 = ap[12];
		a1 = ap[13];
		a2 = ap[14];
		a3 = ap[15];

		cp[12] = a0 * bp[0] + a1 * bp[4] + a2 * bp[8] + a3 * bp[12];
		cp[13] = a0 * bp[1] + a1 * bp[5] + a2 * bp[9] + a3 * bp[13];
		cp[14] = a0 * bp[2] + a1 * bp[6] + a2 * bp[10] + a3 * bp[14];
		cp[15] = a0 * bp[3] + a1 * bp[7] + a2 * bp[11] + a3 * bp[15];
#endif

	}

	//返回一个转置后的新矩阵
	Matrix44 transposed() const
	{
#if 0
		Matrix44 transpMat;

		for (uint8_t i = 0; i < 4; ++i)
		{
			for (uint8_t j = 0; j < 4; ++j)
			{
				transpMat[i][j] = m[j][i];
			}
		}
		return transpMat;
#else
		//这个写法和上面的功能是一样的
		return Matrix44(m[0][0],
						m[1][0],
						m[2][0],
						m[3][0],
						m[0][1],
						m[1][1],
						m[2][1],
						m[3][1],
						m[0][2],
						m[1][2],
						m[2][2],
						m[3][2],
						m[0][3],
						m[1][3],
						m[2][3],
						m[3][3]);
#endif
	}

	//将自身的矩阵进行转换，并返回自身的引用
	Matrix44& transpose()
	{
		Matrix44 tmp(
			m[0][0],
			m[1][0],
			m[2][0],
			m[3][0],
			m[0][1],
			m[1][1],
			m[2][1],
			m[3][1],
			m[0][2],
			m[1][2],
			m[2][2],
			m[3][2],
			m[0][3],
			m[1][3],
			m[2][3],
			m[3][3],
			);

		*this = tmp;

		return *this;
	}

	//左乘，point和matrix相乘，point的w向量为‘1’
	void multVecMatrix(const Vec3<T> &src, Vec3<T> &dst) const
	{
		dst.x = src.x*m[0][0] + src.y*m[1][0] + src.z*m[2][0] + m[3][0];
		dst.y = src.x*m[0][1] + src.y*m[1][1] + src.z*m[2][1] + m[3][1];
		dst.z = src.x*m[0][2] + src.y*m[1][2] + src.z*m[2][2] + m[3][2];

		//这一步只有在做透视投影时才用到，用来将与透视投影矩阵相乘得到坐标转换为NDC坐标
		T w = src.x*m[0][3] + src.y*m[1][3] + src.z*m[2][3] + m[3][3];
		if (w != 1 && w != 0)
		{
			dst.x /= w;
			dst.y /= w;
			dst.z /= w;
		}
	}

	//左乘，vector和matrix相乘
	void multDirMatrix(const Vec3<T> &src, Vec<T> &dst) const
	{
		dst.x = src.x*m[0][0] + src.y*m[1][0] + src.z*m[2][0];
		dst.y = src.x*m[0][1] + src.y*m[1][1] + src.z*m[2][1];
		dst.z = src.x*m[0][2] + src.y*m[1][2] + src.z*m[2][2];
	}

	//矩阵的转置
	Matrix44 inverse()
	{
		int i, j, k;
		Matrix44 s;
		Matrix44 t(*this);

		// Forward elimination
		for (i = 0; i < 3; i++) {
			int pivot = i;

			T pivotsize = t[i][i];

			if (pivotsize < 0)
				pivotsize = -pivotsize;

			for (j = i + 1; j < 4; j++) {
				T tmp = t[j][i];

				if (tmp < 0)
					tmp = -tmp;

				if (tmp > pivotsize) {
					pivot = j;
					pivotsize = tmp;
				}
			}

			if (pivotsize == 0) {
				// Cannot invert singular matrix
				return Matrix44();
			}

			if (pivot != i) {
				for (j = 0; j < 4; j++) {
					T tmp;

					tmp = t[i][j];
					t[i][j] = t[pivot][j];
					t[pivot][j] = tmp;

					tmp = s[i][j];
					s[i][j] = s[pivot][j];
					s[pivot][j] = tmp;
				}
			}

			for (j = i + 1; j < 4; j++) {
				T f = t[j][i] / t[i][i];

				for (k = 0; k < 4; k++) {
					t[j][k] -= f * t[i][k];
					s[j][k] -= f * s[i][k];
				}
			}
		}

		// Backward substitution
		for (i = 3; i >= 0; --i) {
			T f;

			if ((f = t[i][i]) == 0) {
				// Cannot invert singular matrix
				return Matrix44();
			}

			for (j = 0; j < 4; j++) {
				t[i][j] /= f;
				s[i][j] /= f;
			}

			for (j = 0; j < i; j++) {
				f = t[j][i];

				for (k = 0; k < 4; k++) {
					t[j][k] -= f * t[i][k];
					s[j][k] -= f * s[i][k];
				}
			}
		}

		return s;
	}

	// \brief set current matrix to its inverse
	const Matrix44<T>& invert()
	{
		*this = inverse();
		return *this;
	}

	friend std::ostream& operator << (std::ostream &s, const Matrix44 &)
	{
		//格式设置
		std::ios_base::fmtflags oldFlags = s.flags();
		int width = 12;
		s.precision(5);
		s.setf(std::ios_base::fixed);

		s << "(" << std::setw(width) << m[0][0] <<
			" " << std::setw(width) << m[0][1] <<
			" " << std::setw(width) << m[0][2] <<
			" " << std::setw(width) << m[0][3] << "\n" <<

			" " << std::setw(width) << m[1][0] <<
			" " << std::setw(width) << m[1][1] <<
			" " << std::setw(width) << m[1][2] <<
			" " << std::setw(width) << m[1][3] << "\n" <<

			" " << std::setw(width) << m[2][0] <<
			" " << std::setw(width) << m[2][1] <<
			" " << std::setw(width) << m[2][2] <<
			" " << std::setw(width) << m[2][3] << "\n" <<

			" " << std::setw(width) << m[3][0] <<
			" " << std::setw(width) << m[3][1] <<
			" " << std::setw(width) << m[3][2] <<
			" " << std::setw(width) << m[3][3] << ")\n";

		//格式恢复
		s.flags(oldFlags);
		return s;
	}

};

typedef Matrix44<float> Matrix44f;

#endif