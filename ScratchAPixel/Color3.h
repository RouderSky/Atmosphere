#ifndef __COLOR3_H__
#define __COLOR3_H__

#include <fstream>
#include <algorithm>

template<typename T>
class Color3
{
public:
	Color3(): r(0), g(0), b(0){}
	Color3(T rr) : r(rr), g(rr), b(rr) {}
	Color3(T rr, T gg, T bb) :r(rr), g(gg), b(bb) {}
	Color3 operator * (const T &v) const
	{
		return Color3(r*v, g*v, b*v);
	}
	friend Color3 operator * (const T &v, const Color3 &c)		//scratchapixel��Tֱ������Ϊfloat����������д����
	{
		return c*v;
	}
	Color3 operator + (const Color3 &c) const
	{
		return Color3(r + c.r, g + c.g, b + c.g);
	}
	friend std::ostream & operator << (std::ostream &os, const Color3 &c)
	{
		os << c.r << " " << c.g << " " << c.g;
		return os;
	}

	T r, g, b;		//scratchapixel��Tֱ������Ϊfloat����������д����
};

typedef Color3<float> Color3f;

//�����������PPM�ļ��У�PPM�ļ�������photoshop��
//fnΪPPM�ļ���
//c�洢���������ص���ɫ
//widthΪͼ���ȣ���λΪ����
//heightΪͼ��߶ȣ���λΪ����
void saveToPPM(const char *fn, const Color3f *c, const int &width, const int &height)
{
	std::ofstream ofs;
	//��������windowsƽ̨��flagҪд�ϣ��������ø�ʽ�ģ������Ҳ�֪����ôд��������������������
	ofs.open(fn, std::ios::out | std::ios::binary);

	if (ofs.fail())
	{
		fprintf(stderr, "ERROR: cant;t save image to file %s\n", fn);	//stderr��ʲô�ļ���
	}
	else
	{
		ofs << "P6\n" << width << " " << height << "\n255\n";
		const Color3f *pc = c;
		for (int i = 0; i < height; ++i)
		{
			for (int j = 0; j < width; ++j)
			{
				char r = static_cast<char>(std::min(255.f, 255 * pc->r + 0.5f));		//Ϊʲôʹ��static_cast��
				char g = static_cast<char>(std::min(255.f, 255 * pc->g + 0.5f));
				char b = static_cast<char>(std::min(255.f, 255 * pc->b + 0.5f));
				ofs << r << g << b;
				++pc;
			}
		}
	}

	ofs.close();
}



#endif
