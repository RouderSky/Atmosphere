#ifndef _SPHERE_H__
#define _SPHERE_H__

//#include "Geometry.h"
#include "Vector.h"

class Sphere
{
public:
	Vec3f center;
	float radius, radius2;
	Vec3f diffuseColor;				//漫反射颜色，几乎所有外来光线都要与这个颜色交互融合
	bool hasTexture;
	Vec3f(*textureColor)(const Vec2f&);
	Vec3f emissionColor;			//自发光颜色，物体表层的一层光线，光线的方向都是法线的方向
	float transparency;				//透明度
	float reflection;				//镜面反射度
									//还可以加上一个折射率属性，线面的球体全部统一为1.1了

	Sphere(
		const Vec3f &c,
		const float &r,
		const Vec3f &dc,
		Vec3f(*tex)(const Vec2f&) = NULL,
		const float &refl = 0,
		const float &transp = 0,
		const Vec3f &ec = 0) :
		center(c), radius(r), radius2(r*r), diffuseColor(dc), textureColor(tex), emissionColor(ec), transparency(transp), reflection(refl)
	{
		hasTexture = textureColor != NULL;
	}

	Vec3f getSurfaceColor(Vec2f uv) const
	{
		if (hasTexture)
			return textureColor(uv);
		else
			return diffuseColor;
	}

	//计算点center到直线raydir的距离d，如果d小于等于radius，那么就相交；
	//最后要返回是否相交，还要通过引用返回相交点与rayorig的距离(注意rayorig可能在物体内部，也可能在物体外部)
	bool intersect(const Vec3f &rayorig, const Vec3f &raydir, float &t)const
	{
		Vec3f l = center - rayorig;
		float projectOflToRaydir = l.dot(raydir);
		if (projectOflToRaydir < 0)
			return false;
		float d2 = l.length2() - projectOflToRaydir*projectOflToRaydir;
		if (d2 > radius2)
			return false;
		float offset = sqrt(radius2 - d2);
		float t0 = projectOflToRaydir - offset;
		float t1 = projectOflToRaydir + offset;

		t = t0 < 0 ? t1 : t0;		//根据发射点在不在物体内部来确定击中距离
		return true;
	}

	void getSurfaceData(const Vec3f &raydir, const Vec3f &Phit, Vec3f &Nhit, bool &inside, Vec2f &uv) const
	{
		Nhit = Phit - center;				//ray击中点的球体外法线
		Nhit.normalize();
		inside = false;							//ray的发射点是不是在内部
		if (raydir.dot(Nhit) > 0)
		{
			Nhit = -Nhit;
			inside = true;
		}

		//uv坐标....................................
		uv.x = (1 + atan2(Nhit.z, Nhit.x) / M_PI) * 0.5;
		uv.y = acosf(Nhit.y) / M_PI;
	}
};

#endif