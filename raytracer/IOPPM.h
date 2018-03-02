#ifndef THINKS_PPM_PPM_H__
#define THINKS_PPM_PPM_H__

#include <cassert>
#include <cstdint>
#include <exception>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <iostream>

#include "Vector.h"
#include "Common.h"

//image中的数值范围：[0,1]
void out2PPM(Vec3f *image, int pixelNumOfWidth, int pixelNumOfHeight, std::string fileName)
{
	//输出到PPM文件
	std::ofstream ofs(fileName, std::ios::out | std::ios::binary);
	ofs << "P6\n" << pixelNumOfWidth << " " << pixelNumOfHeight << "\n255\n";
	//ofs << "P6\n" << pixelNumOfWidth << " " << pixelNumOfHeight;
	for (unsigned i = 0; i < pixelNumOfWidth*pixelNumOfHeight; ++i)
	{
		//四色五入
		ofs << (unsigned char)(clamp(image[i].x, 0.f, 1.f) * 255 + 0.5)
			<< (unsigned char)(clamp(image[i].y, 0.f, 1.f) * 255 + 0.5)
			<< (unsigned char)(clamp(image[i].z, 0.f, 1.f) * 255 + 0.5);
	}
	ofs.close();
}

template<typename F> inline
F openFileStream(
	std::string const& filename,
	std::ios_base::openmode const mode = std::ios::binary)
{
	using namespace std;

	auto ofs = F(filename, mode);
	if (ofs.fail()) {
		auto ss = stringstream();
		ss << "cannot open file '" << filename << "'";
		throw runtime_error(ss.str());
	}

	return ofs;
}

inline
std::vector<std::uint8_t>* readRgbImage(
	std::string const& filename,
	std::size_t* width,
	std::size_t* height,
	std::size_t* maxValue)
{
	using namespace std;

	assert(width != nullptr);
	assert(height != nullptr);

	auto is = openFileStream<std::ifstream>(filename);

	// Read header.
	auto const expected_magic_number = string("P6");
	auto const expected_max_value = string("255");
	auto magic_number = string();
	auto max_value = string();
	is >> magic_number >> *width >> *height >> max_value;
	*maxValue = atoi(max_value.c_str());

	assert(*width != 0);
	assert(*height != 0);

	if (magic_number != expected_magic_number) {
		auto ss = stringstream();
		ss << "magic number must be '" << expected_magic_number << "'";
		throw runtime_error(ss.str());
	}

	if (max_value != expected_max_value) {
		auto ss = stringstream();
		ss << "max value must be " << expected_max_value;
		throw runtime_error(ss.str());
	}

	// Skip ahead (an arbitrary number!) to the pixel data.
	is.ignore(256, '\n');

	std::vector<std::uint8_t>* pixel_data = new std::vector<uint8_t>();
	// Read pixel data.
	pixel_data->resize((*width) * (*height) * 3);
	is.read(reinterpret_cast<char*>(pixel_data->data()), pixel_data->size());

	if (!is) {
		auto ss = stringstream();
		ss << "failed reading " << pixel_data->size() << " bytes";
		throw runtime_error(ss.str());
	}

	is.close();

	return pixel_data;
}

#endif