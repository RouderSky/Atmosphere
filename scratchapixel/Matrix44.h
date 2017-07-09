template<typename T>
class Matrix44
{
public:
	Matrix44(){}

	//重载访问操作符
	//注意：即使函数的参数列表相同，返回值加上const也算是一种重载
	//uint8_t是什么数据类型？？？？？？？？？？？？？？
	const T* operator [] (uint8_t i) const{ return m[i]; }		//传出的数组不可改变
	T* operator [] (uint8_t i) const{ return m[i]; }			//传出的数组可以改变

	//将矩阵默认初始化为单位矩阵
	T m[4][4] = { { 1, 0, 0, 0 }, { 0, 1, 0, 0 }, { 0, 0, 1, 0 }, { 0, 0, 0, 1 } };

	//这个函数就奇怪了，为什么Matrix44不用声明类型T了？？？？？
	Matrix44 operator * (const Matrix44& rhs) const
	{
		Matrix44 mult;
		for (uint8_t i = 0; i < 4; ++i)
		{
			for (uint8_t j = 0; j < 4; ++j)
			{
				mult[i][j] = m[i][0] * m[0][j] +
					m[i][1] * m[1][j] +
					m[i][2] * m[2][j] +
					m[i][3] * m[3][j];
			}
		}
		return mult;
	}

	//左乘
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

	//左乘
	void multDirMatrix(const Vec3<T> &src, Vec<T> &dst) const
	{
		dst.x = src.x*m[0][0] + src.y*m[1][0] + src.z*m[2][0];
		dst.y = src.x*m[0][1] + src.y*m[1][1] + src.z*m[2][1];
		dst.z = src.x*m[0][2] + src.y*m[1][2] + src.z*m[2][2];
	}

	//转置
	Matrix44 transpose() const
	{
		Matrix44 transpMat;

		for (uint8_t i = 0; i < 4; ++i)
		{
			for (uint8_t j = 0; j < 4;++j)
			{
				transpMat[i][j] = m[j][i];
			}
		}
		return transpMat;
	}

};

typedef Matrix44<float> Matrix44f;