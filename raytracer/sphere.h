#ifndef _SPHERE_H__
#define _SPHERE_H__

//#include "Geometry.h"
#include "Vector.h"

class Sphere
{
public:
	Vec3f center;
	float radius, radius2;
	Vec3f diffuseColor;				//��������ɫ�����������������߶�Ҫ�������ɫ�����ں�
	bool hasTexture;
	Vec3f(*textureColor)(const Vec2f&);
	Vec3f emissionColor;			//�Է�����ɫ���������һ����ߣ����ߵķ����Ƿ��ߵķ���
	float transparency;				//͸����
	float reflection;				//���淴���
									//�����Լ���һ�����������ԣ����������ȫ��ͳһΪ1.1��

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

	//�����center��ֱ��raydir�ľ���d�����dС�ڵ���radius����ô���ཻ��
	//���Ҫ�����Ƿ��ཻ����Ҫͨ�����÷����ཻ����rayorig�ľ���(ע��rayorig�����������ڲ���Ҳ�����������ⲿ)
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

		t = t0 < 0 ? t1 : t0;		//���ݷ�����ڲ��������ڲ���ȷ�����о���
		return true;
	}

	void getSurfaceData(const Vec3f &raydir, const Vec3f &Phit, Vec3f &Nhit, bool &inside, Vec2f &uv) const
	{
		Nhit = Phit - center;				//ray���е�������ⷨ��
		Nhit.normalize();
		inside = false;							//ray�ķ�����ǲ������ڲ�
		if (raydir.dot(Nhit) > 0)
		{
			Nhit = -Nhit;
			inside = true;
		}

		//uv����....................................
		uv.x = (1 + atan2(Nhit.z, Nhit.x) / M_PI) * 0.5;
		uv.y = acosf(Nhit.y) / M_PI;
	}
};

#endif