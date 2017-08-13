#if 0
float bilinear(
	const float &tx,
	const float  &ty,
	const Vec3f &c00,
	const Vec3f &c10,
	const Vec3f &c01,
	const Vec3f &c11)
{
#if 0 
	float  a = c00 * (1 - tx) + c10 * tx;
	float  b = c01 * (1 - tx) + c11 * tx;
	return a * (1) - ty) + b * ty;
#else	//这种形式比较好理解
	return (1 - tx) * (1 - ty) * c00 +		//注意：离得越远就权值就越小
		tx * (1 - ty) * c10 +
		(1 - tx) * ty * c01 +
		tx * ty * c11;
#endif 
}

//将一张经过插值放大
void testBilinearInterpolation()
{
	// testing bilinear interpolation
	int imageWidth = 512;
	int gridSizeX = 9, gridSizeY = 9;		//格子的个数，这个设计真烂；原像素的个数就是10
	Vec3f *grid2d = new Vec3f[(gridSizeX + 1) * (gridSizeY + 1)]; // lattices 
																  // fill grid with random colors
	for (int j = 0, k = 0; j <= gridSizeY; ++j) {
		for (int i = 0; i <= gridSizeX; ++i, ++k) {
			grid2d[j * (gridSizeX + 1) + i] = Vec3f(drand48(), drand48(), drand48());		//为原像素设定随机颜色值
		}
	}

	//得到一张图片
	//将这张图片进行放大，由原来的10*10扩大到512*512

	// now compute our final image using bilinear interpolation
	Vec3f *imageData = new Vec3f[imageWidth*imageWidth], *pixel = imageData;
	for (int j = 0; j < imageWidth; ++j) {
		for (int i = 0; i < imageWidth; ++i) {
			// convert i,j to grid coordinates
			T gx = i / float(imageWidth) * gridSizeX; // be careful to interpolate boundaries 
			T gy = j / float(imageWidth) * gridSizeY; // be careful to interpolate boundaries 
			int gxi = int(gx);
			int gyi = int(gy);
			const Vec3f & c00 = grid2d[gyi * (gridSizeX + 1) + gxi];
			const Vec3f & c10 = grid2d[gyi * (gridSizeX + 1) + (gxi + 1)];
			const Vec3f & c01 = grid2d[(gyi + 1) * (gridSizeX + 1) + gxi];
			const Vec3f & c11 = grid2d[(gyi + 1) * (gridSizeX + 1) + (gxi + 1)];
			*(pixel++) = bilinear(gx - gxi, gy - gyi, c00, c10, c01, c11);
		}
	}
	saveToPPM("./bilinear.ppm", imageData, imageWidth, imageWidth);
	delete[] imageData;
}

#endif