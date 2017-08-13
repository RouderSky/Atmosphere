#ifndef __INTERPOLATION_H__
#define __INTERPOLATION_H__

#ifdef WIN32
#include "stdafx.h"
#endif

#include <cstdio>
#include <cstdlib>
#include "Color3.h"

//Bilinear interpolation 
//tx=x/|c10-c00|
//ty=y/|c01-c00|
//ps：x是需要插值的点到c00的水平距离
//	  y是需要插值的点到c00的垂直距离
template<typename T>
T bilinear(
	const float &tx,
	const float &ty,
	const T &c00,
	const T &c10,
	const T &c01,
	const T &c11)
{
#if 0
	//先对x轴方向进行插值
	T a = c00*(1.f - tx) + c10*tx;
	T b = c01*(1.f - tx) + c11*tx;

	//再将x轴方向上插值的结果a和b进行一次y轴方向上的插值
	return a*(1.f - ty) + b*ty;
#else
	//直接对四点的数据进行加权相加
	return (1 - tx)*(1 - ty)*c00 +
		tx*(1 - ty)*c10 +
		(1 - tx)*ty*c01 +
		tx*ty*c11;
#endif
}


//testCode
#ifdef WIN32
#define RANDFLOAT float(rand())/RAND_MAX
#else
#define RANDFLOAT drand48()
#endif

//具体要完成的功能到网站查看，但是注意：网站中的图片有问题，多出了一行一列
void testBilinearInterpolation()
{
	//x，y轴上的格子个数
	int gridSizeX = 9, gridSizeY = 9;			
	//格子的顶点数据
	Color3f *vertexData = new Color3f[(gridSizeX + 1)*(gridSizeY + 1)];		
	//给格子的顶点赋值
	for (int i = 0; i < gridSizeX + 1; ++i)
	{
		for (int j = 0; j < gridSizeY + 1; ++j)
		{
			vertexData[i*(gridSizeX + 1) + j] = Color3f(RANDFLOAT, RANDFLOAT, RANDFLOAT);
			printf("%d %d %f %f %f\n", i, j, vertexData[i*(gridSizeX + 1) + j].r, vertexData[i*(gridSizeX + 1) + j].g, vertexData[i*(gridSizeX + 1) + j].b);
		}
	}

	//开始插值出图像
	int imageWidth = 512;
	int imageHeight = 512;
	Color3f *imageData = new Color3f[imageWidth*imageHeight];	//图像的像素数据
	Color3f *pixel = imageData;
	for (int i = 0; i < imageWidth; ++i)
	{
		for (int j = 0; j < imageHeight; ++j)
		{
			//确定该像素所属格子
			float gx = i / (float)imageWidth * gridSizeX;
			float gy = j / (float)imageHeight * gridSizeY;
			int gxi = (int)gx;
			int gyi = (int)gy;
			//获取格子四个顶点的数据
			const Color3f & c00 = vertexData[(gyi + 1)*(gridSizeX+1)+gxi];
			const Color3f & c10 = vertexData[(gyi + 1)*(gridSizeX + 1) + (gxi+1)];
			const Color3f & c01 = vertexData[gyi*(gridSizeX + 1) + gxi];
			const Color3f & c11 = vertexData[gyi*(gridSizeX + 1) + (gxi+1)];
			*(pixel++) = bilinear(gx - gxi, 1 - (gy - gyi), c00, c10, c01, c11);
		}
	}
	//输出到PPM文件
	saveToPPM("./bilinear.ppm", imageData, imageWidth, imageHeight);


	//不进行双线性插值的图像
	pixel = imageData;
	int cellsize = imageWidth / (gridSizeX);
	fprintf(stderr, "%d\n", cellsize);
	for (int j = 0; j < imageWidth; ++j) {
		for (int i = 0; i < imageWidth; ++i) {
			float gx = (i + cellsize / 2) / float(imageWidth);
			float gy = (j + cellsize / 2) / float(imageWidth);
			int gxi = static_cast<int>(gx * gridSizeX);
			int gyi = static_cast<int>(gy * gridSizeY);
			*pixel = vertexData[gyi * (gridSizeX + 1) + gxi];
			int mx = (i + cellsize / 2) % cellsize;
			int my = (j + cellsize / 2) % cellsize;
			int ma = cellsize / 2 + 2, mb = cellsize / 2 - 2;
			if (mx < ma && mx > mb && my < ma && my > mb)
				*pixel = Color3f(0, 0, 0);
			pixel++;
		}
	}
	saveToPPM("./origin.ppm", imageData, imageWidth, imageWidth);

	delete[] imageData;
}


#endif
