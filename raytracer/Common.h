#ifndef __COMMON_H__
#define __COMMON_H__


template<typename T>
inline T clamp(const T &v, const T &lo, const T &hi)
{
	return std::max(lo, std::min(hi, v));
}


#endif