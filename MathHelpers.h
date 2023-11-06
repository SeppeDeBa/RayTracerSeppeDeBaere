#pragma once
#include <cmath>
#include <float.h>

namespace dae
{
	/* --- CONSTANTS --- */
	constexpr auto PI = 3.14159265358979323846f;
	constexpr auto PI_DIV_2 = 1.57079632679489661923f;
	constexpr auto PI_DIV_4 = 0.785398163397448309616f;
	constexpr auto PI_2 = 6.283185307179586476925f;
	constexpr auto PI_4 = 12.56637061435917295385f;

	constexpr auto TO_DEGREES = (180.0f / PI);
	constexpr auto TO_RADIANS(PI / 180.0f);

	inline float Square(float a)
	{
		return a * a;
	}

	inline float Lerpf(float a, float b, float factor)
	{
		return ((1 - factor) * a) + (factor * b);
	}

	inline bool AreEqual(float a, float b, float epsilon = FLT_EPSILON)
	{
		return abs(a - b) < epsilon;
	}

	//TODO: Ask if this is fine
	inline float Quake3Sqrt(float number) //https://www.youtube.com/watch?v=p8u_k2LIZyo&t=170s&ab_channel=Nemean link to video, can check on wikipedia aswell
	{
		long i;
		float x2, y;
		const float threehalfs = 1.5f;

		x2 = number * 0.5f;
		y = number;

		i = *(long*)&y;
		i = 0x5f3759df - (i >> 1);

		y = *(float*)&i;
		y = y * (threehalfs - (x2 * y * y));
		//can do more iterations but 1 is fine.
		//y = y * (threehalfs - (x2 * y * y));
		return y;
	}
}