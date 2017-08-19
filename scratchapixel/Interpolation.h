#ifndef __INTERPOLATION_H__
#define __INTERPOLATION_H__

#ifdef WIN32
#include "stdafx.h"
#endif

#include <cstdio>
#include <cstdlib>
#include "Color3.h"

//ʹ�����Բ�ֵ����ǰ�Ȼ���ͼ�����������ϵ

//Bilinear interpolation 
//����ʹ�õ�����ϵ��x�����������ң�y�����������ϣ�
//tx,tyֻ�������������Χ��[0,1]
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
	//�ȶ�x�᷽����в�ֵ
	T a = c00*(1.f - tx) + c10*tx;
	T b = c01*(1.f - tx) + c11*tx;

	//�ٽ�x�᷽���ϲ�ֵ�Ľ��a��b����һ��y�᷽���ϵĲ�ֵ
	return a*(1.f - ty) + b*ty;
#else
	//ֱ�Ӷ��ĵ�����ݽ��м�Ȩ���
	return (1 - tx)*(1 - ty)*c00 +
		tx*(1 - ty)*c10 +
		(1 - tx)*ty*c01 +
		tx*ty*c11;
#endif
}

//Trilinear interpolation
//����ʹ�õ�����ϵ��x�����ң�y�����ϣ�z����ǰ����һ����������ϵ��
//tx,ty,tzֻ�������������Χ��[0,1]
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
	//�ȶ�x��y�᷽�����˫���Բ�ֵ
	T e = bilinear(tx, ty, c000, c100, c010, c110);
	T f = bilinear(tx, ty, c001, c101, c011, c111);

	//�ٽ�x��y�᷽���ֵ�Ľ������һ��z�����ϵĲ�ֵ
	return (1 - tz)*e + tz*f;
#else
	//ֱ�Ӷ԰˵�����ݽ��м�Ȩ���
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

//����Ҫ��ɵĹ��ܵ���վ�鿴������ע�⣺��վ�е�ͼƬ�����⣬�����һ��һ��
void testBilinearInterpolation()
{
	//x��y���ϵĸ��Ӹ���
	int gridSizeX = 9, gridSizeY = 9;			
	//���ӵĶ�������
	Color3f *vertexData = new Color3f[(gridSizeX + 1)*(gridSizeY + 1)];		//��������ԭ��������grid2d����֪��������ʲô
	//�����ӵĶ��㸳ֵ
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
	Color3f *imageData = new Color3f[imageWidth*imageHeight];	//�洢ͼ�����������
	Color3f *pixel = NULL;

	//������˫���Բ�ֵ��ͼ�񣬽������������ȡ��������
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
			int gxi = (int)(gx + 0.5);		//��������
			int gyi = (int)(gy + 0.5);
			*pixel = vertexData[gyi*(gridSizeX + 1) + gxi];		//ֱ������������ɫ
			//��ÿһ�����Ӷ��㻭��һ���ڵ�
			int mx = (j + 1) % cellSizeX;
			int my = (i + 1) % cellSizeY;
			if(mx>=0&&mx<=2&&my>=0&&my<=2)
				*pixel = Color3f(0, 0, 0);
			++pixel;
		}
	}
	saveToPPM("./origin.ppm", imageData, imageWidth, imageHeight);

	//��ʼ��ֵ��ͼ��
	pixel = imageData;
	for (int i = 0; i < imageWidth; ++i)
	{
		for (int j = 0; j < imageHeight; ++j)
		{
			//ȷ����������������
			float gx = j / (float)imageWidth * gridSizeX;
			float gy = i / (float)imageHeight * gridSizeY;
			int gxi = (int)gx;
			int gyi = (int)gy;
			//��ȡ�����ĸ����������
			const Color3f & c00 = vertexData[(gyi + 1)*(gridSizeX + 1)+gxi];
			const Color3f & c10 = vertexData[(gyi + 1)*(gridSizeX + 1) + (gxi+1)];
			const Color3f & c01 = vertexData[gyi*(gridSizeX + 1) + gxi];
			const Color3f & c11 = vertexData[gyi*(gridSizeX + 1) + (gxi+1)];
			*(pixel++) = bilinear(gx - gxi, 1 - (gy - gyi), c00, c10, c01, c11);
		}
	}
	//�����PPM�ļ�
	saveToPPM("./bilinear.ppm", imageData, imageWidth, imageHeight);


	delete[] imageData;
}


#define IX(size,i,j,k)(i*size*size+j*size+k)
void testTrilinearInterpoplation()
{
	//һ����10*10*10��3dС��������ɵĴ�������
	int gridSize = 10;
	int numVertices = gridSize + 1;
	Color3f *vertexData = new Color3f[numVertices* numVertices* numVertices];	//��վ������д����grid3d
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

	//��ֵ�������������������
	float px, py, pz;
	float gx, gy, gz;
	int gxi, gyi, gzi;
	float tx, ty, tz;
	for (int i = 0; i < 10e2; ++i)	//����10*(10^2)��
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
