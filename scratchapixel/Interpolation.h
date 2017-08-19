#ifndef __INTERPOLATION_H__
#define __INTERPOLATION_H__

#ifdef WIN32
#include "stdafx.h"
#endif

#include <cstdio>
#include <cstdlib>
#include "Color3.h"

//使用线性插值函数前先画个图搞清楚坐标体系

//Bilinear interpolation 
//这里使用的坐标系：x轴正方向向右，y轴正方向向上；
//tx,ty只是坐标比例，范围是[0,1]
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

//Trilinear interpolation
//这里使用的坐标系：x轴向右，y轴向上，z轴向前，是一个左手坐标系；
//tx,ty,tz只是坐标比例，范围是[0,1]
template<typename T>
T trilinear(
	const float &tx,
	const float &ty,
	const float &tz,
	const T &c000,
	const T &c100,
	const T &c010,
	const T &c110,
	const T &c001,
	const T &c101,
	const T &c011,
	const T &c111)
{
#if 0
	//先对x、y轴方向进行双线性插值
	T e = bilinear(tx, ty, c000, c100, c010, c110);
	T f = bilinear(tx, ty, c001, c101, c011, c111);

	//再将x、y轴方向插值的结果进行一次z方向上的插值
	return (1 - tz)*e + tz*f;
#else
	//直接对八点的数据进行加权相加
	return (1 - tx)*(1 - ty)*(1 - tz)*c000 +
		         tx*(1 - ty)*(1 - tz)*c100 +
		         (1 - tx)*ty*(1 - tz)*c010 +
		               tx*ty*(1 - tz)*c110 +
		         (1 - tx)*(1 - ty)*tz*c001 +
		               tx*(1 - ty)*tz*c101 +
		               (1 - tx)*ty*tz*c011 +
		                     tx*ty*tz*c111;
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
	Color3f *vertexData = new Color3f[(gridSizeX + 1)*(gridSizeY + 1)];		//这个数组的原本命名是grid2d，不知道意义是什么
	//给格子的顶点赋值
	for (int i = 0; i < gridSizeX + 1; ++i)
	{
		for (int j = 0; j < gridSizeY + 1; ++j)
		{
			vertexData[i*(gridSizeX + 1) + j] = Color3f(RANDFLOAT, RANDFLOAT, RANDFLOAT);
			printf("%d %d %f %f %f\n", i, j, vertexData[i*(gridSizeX + 1) + j].r, vertexData[i*(gridSizeX + 1) + j].g, vertexData[i*(gridSizeX + 1) + j].b);
		}
	}

	int imageWidth = 512;
	int imageHeight = 512;
	Color3f *imageData = new Color3f[imageWidth*imageHeight];	//存储图像的像素数据
	Color3f *pixel = NULL;

	//不进行双线性插值的图像，进行四舍五入获取顶点数据
	pixel = imageData;
	int cellSizeX = imageWidth / gridSizeX;
	int cellSizeY = imageHeight / gridSizeY;
	fprintf(stderr, "%d %d\n", cellSizeX, cellSizeY);
	for (int i = 0; i < imageWidth; ++i)
	{
		for (int j = 0; j < imageHeight; ++j)
		{
			float gx = j / (float)imageWidth * gridSizeX;
			float gy = i / (float)imageHeight * gridSizeY;
			int gxi = (int)(gx + 0.5);		//四舍五入
			int gyi = (int)(gy + 0.5);
			*pixel = vertexData[gyi*(gridSizeX + 1) + gxi];		//直接设置像素颜色
			//在每一个格子顶点画上一个黑点
			int mx = (j + 1) % cellSizeX;
			int my = (i + 1) % cellSizeY;
			if(mx>=0&&mx<=2&&my>=0&&my<=2)
				*pixel = Color3f(0, 0, 0);
			++pixel;
		}
	}
	saveToPPM("./origin.ppm", imageData, imageWidth, imageHeight);

	//开始插值出图像
	pixel = imageData;
	for (int i = 0; i < imageWidth; ++i)
	{
		for (int j = 0; j < imageHeight; ++j)
		{
			//确定该像素所属格子
			float gx = j / (float)imageWidth * gridSizeX;
			float gy = i / (float)imageHeight * gridSizeY;
			int gxi = (int)gx;
			int gyi = (int)gy;
			//获取格子四个顶点的数据
			const Color3f & c00 = vertexData[(gyi + 1)*(gridSizeX + 1)+gxi];
			const Color3f & c10 = vertexData[(gyi + 1)*(gridSizeX + 1) + (gxi+1)];
			const Color3f & c01 = vertexData[gyi*(gridSizeX + 1) + gxi];
			const Color3f & c11 = vertexData[gyi*(gridSizeX + 1) + (gxi+1)];
			*(pixel++) = bilinear(gx - gxi, 1 - (gy - gyi), c00, c10, c01, c11);
		}
	}
	//输出到PPM文件
	saveToPPM("./bilinear.ppm", imageData, imageWidth, imageHeight);


	delete[] imageData;
}


#define IX(size,i,j,k)(i*size*size+j*size+k)
void testTrilinearInterpoplation()
{
	//一个由10*10*10个3d小立方体组成的大立方体
	int gridSize = 10;
	int numVertices = gridSize + 1;
	Color3f *vertexData = new Color3f[numVertices* numVertices* numVertices];	//网站上这里写的是grid3d
	for (int i = 0; i < numVertices; ++i)
	{
		for (int j = 0; j < numVertices; ++j)
		{
			for (int k = 0; k < numVertices; ++k)
			{
				vertexData[IX(numVertices, i, j, k)] = Color3f(RANDFLOAT, RANDFLOAT, RANDFLOAT);
			}
		}
	}

	//插值出大立方体的所有数据
	float px, py, pz;
	float gx, gy, gz;
	int gxi, gyi, gzi;
	float tx, ty, tz;
	for (int i = 0; i < 10e2; ++i)	//计算10*(10^2)次
	{
		px = RANDFLOAT;
		py = RANDFLOAT;
		pz = RANDFLOAT;
		gx = px*gridSize; gxi = (int)gx; tx = gx - gxi;
		gy = py*gridSize; gyi = (int)gy; ty = gy - gyi;
		gz = pz*gridSize; gzi = (int)gz; tz = gz - gzi;
		const Color3f & c000 = vertexData[IX(numVertices, gxi, gyi + 1, gzi)];
		const Color3f & c010 = vertexData[IX(numVertices, gxi+1, gyi + 1, gzi)];
		const Color3f & c100 = vertexData[IX(numVertices, gxi, gyi, gzi)];
		const Color3f & c110 = vertexData[IX(numVertices, gxi+1, gyi, gzi)];
		const Color3f & c001 = vertexData[IX(numVertices, gxi, gyi + 1, gzi+1)];
		const Color3f & c101 = vertexData[IX(numVertices, gxi+1, gyi + 1, gzi+1)];
		const Color3f & c011 = vertexData[IX(numVertices, gxi, gyi + 1, gzi+1)];
		const Color3f & c111 = vertexData[IX(numVertices, gxi+1, gyi, gzi+1)];
		trilinear(tx, 1 - ty, tz, c000, c100, c010, c110, c001, c101, c010, c111);
	}

	delete[] vertexData;
}


#endif
