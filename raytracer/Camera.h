#ifndef _CAMERA_H__
#define _CAMERA_H__

#include <iostream>
#include "Vector.h"
#include "Matrix44.h"

//摄像机坐标：右手系
class Camera
{
public:
	Vec3f position;
	Vec3f dir;
	unsigned pixelNumOfWidth;
	unsigned pixelNumOfHeight;
	float fov;
	float canvasZ = -1;
	Matrix44f MCam2World;

	Camera(const Vec3f &position, const Vec3f &dir,const unsigned &pixelNumOfWidth, const unsigned &pixelNumOfHeight, const float &fov):
		position(position),
		dir(dir),
		pixelNumOfWidth(pixelNumOfWidth),
		pixelNumOfHeight(pixelNumOfHeight),
		fov(fov)
	{
		MCam2World = lookAt();
	}


	//通过屏幕坐标计算出摄像机向量，再转换到世界向量
	Vec3f getCameraRayInWorld(float x, float y)
	{
		//确定每一个像素的位置，然后确定primary ray的方向
		//这里我们假定：摄像机坐标系与世界坐标系完全重合，所以我们不需要坐标系转换矩阵；且cavas这个平面位于z=-1这个平面上
		float aspectratio = pixelNumOfWidth / (float)pixelNumOfHeight;
		float height = 2 * tan(fov / 2 * M_PI / 180);		//canvas宽度
		float width = height * aspectratio;					//canvas高度
		float widthOfPixel = width / pixelNumOfWidth;		//canvas上的像素宽度
		float heightOfPixel = height / pixelNumOfHeight;	//canvas上的像素高度

		//屏幕空间原点在摄像机空间的位置
		float x0 = -(width / 2);
		float y0 = -(height / 2);

		Vec3f rayInCamera(x0 + x*widthOfPixel, y0 + y*heightOfPixel, canvasZ);
		Vec3f rayInWorld;
		MCam2World.multDirMatrix(rayInCamera, rayInWorld);
		return rayInWorld;
	}

	//左乘
	Matrix44f lookAt()
	{
		Vec3f forward = -dir;
		forward.normalize();
		Vec3f right = Vec3f(0, 1, 0).crossProduct(forward);
		Vec3f up = forward.crossProduct(right);

		Matrix44f camToWorld;

		camToWorld[0][0] = right.x;
		camToWorld[0][1] = right.y;
		camToWorld[0][2] = right.z;
		camToWorld[1][0] = up.x;
		camToWorld[1][1] = up.y;
		camToWorld[1][2] = up.z;
		camToWorld[2][0] = forward.x;
		camToWorld[2][1] = forward.y;
		camToWorld[2][2] = forward.z;

		camToWorld[3][0] = position.x;
		camToWorld[3][1] = position.y;
		camToWorld[3][2] = position.z;

		return camToWorld;
	}

};
#endif // _CAMERA_H__