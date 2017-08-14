// scratchapixel.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#include "Vector.h"
#include "Matrix44.h"
#include "Interpolation.h"

int main()
{
#if 0
	Vec3f v(0, 1, 2);
	std::cerr << v << std::endl;
	Matrix44f a, b, c;
	c = a * b;

	Matrix44f d(0.707107, 0, -0.707107, 0, -0.331295, 0.883452, -0.331295, 0, 0.624695, 0.468521, 0.624695, 0, 4.000574, 3.00043, 4.000574, 1);
	std::cerr << d << std::endl;
	d.inverse();
	std::cerr << d << std::endl;
#endif

//	testBilinearInterpolation();
	testTrilinearInterpoplation();

	system("pause");
	return 0;
}

