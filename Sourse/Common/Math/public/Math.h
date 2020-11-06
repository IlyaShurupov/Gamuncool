#pragma once

#include <stdio.h>
#include <math.h>

#define Pi float(3.1415926535897932384626433832795)
#define INF long long int (4611686018427387904)

#define CLAMP(val, min, max) \ if (*val > max) *val = max; if (*val < min) *val = min;
#define DOT3(v1, v2) float(v1[0] * v2[0] + v1[1] * v2[1] + v1[2] * v2[2])
#define DOT2(v1, v2) float(v1[0] * v2[0] + v1[1] * v2[1])
#define ABS(val) (val > 0) ? val : -val

namespace Math {

	inline void Cross(float out[3], float v1[3], float v2[3])
	{
		out[0] = v1[1] * v2[2] - v1[2] * v2[1];
		out[1] = v1[2] * v2[0] - v1[0] * v2[2];
		out[2] = v1[0] * v2[1] - v1[1] * v2[0];
	}

	void Scale(float v1[3], float s)
	{
		v1[0] *= s;
		v1[1] *= s;
		v1[2] *= s;
	}

	void Add(float v[3], float v1[3], float v2[3])
	{
		v[0] = v1[0] + v2[0];
		v[1] = v1[1] + v2[1];
		v[2] = v1[2] + v2[2];
	}

	void Subtruct(float v[3], float v1[3], float v2[3])
	{
		v[0] = v1[0] - v2[0];
		v[1] = v1[1] - v2[1];
		v[2] = v1[2] - v2[2];
	}

	void RotateZAroundCenter(float v1[3], float angel)
	{
		float sinx = float(sin(angel));
		float cosx = float(cos(angel));
		float vnew[2];
		vnew[0] = v1[0] * cosx - v1[1] * sinx;
		vnew[1] = v1[0] * sinx + v1[1] * cosx;
		v1[0] = vnew[0];
		v1[1] = vnew[1];
	}

	void RotateYAroundCenter(float v1[3], float angel)
	{
		float sinx = float(sin(angel));
		float cosx = float(cos(angel));
		float vnew[2];
		vnew[0] = v1[0] * cosx - v1[2] * sinx;
		vnew[1] = v1[0] * sinx + v1[2] * cosx;
		v1[0] = vnew[0];
		v1[2] = vnew[1];
	}

	void RotateXAroundCenter(float v1[3], float angel)
	{
		float vnew[2];
		float sinx = float(sin(angel));
		float cosx = float(cos(angel));
		vnew[0] = v1[1] * cosx - v1[2] * sinx;
		vnew[1] = v1[1] * sinx + v1[2] * cosx;
		v1[1] = vnew[0];
		v1[2] = vnew[1];
	}

	void RotateArbAxis(float RotVector[3], float axs_pos[3], float Point[3], float rot_angel)
	{

		Subtruct(Point, Point, axs_pos);

		float z_angel = float(atan(RotVector[1] / RotVector[0]));
		if (RotVector[1] > 0 && RotVector[0] < 0)
		{
			z_angel = (Pi + z_angel);
		}
		else
		{
			if (RotVector[1] < 0 && RotVector[0] < 0)
			{
				z_angel = -(Pi - z_angel);
			}
		}

		float c = float(RotVector[0] * RotVector[0] + RotVector[1] * RotVector[1]);
		float y_angel = float(atan(RotVector[2] / sqrt(c)));

		RotateZAroundCenter(Point, -z_angel);
		RotateYAroundCenter(Point, -y_angel);

		RotateXAroundCenter(Point, rot_angel);

		RotateYAroundCenter(Point, y_angel);
		RotateZAroundCenter(Point, z_angel);

		Add(Point, Point, axs_pos);
	}

	float MATH_VectorMagnetude(float v[3])
	{
		float s = v[0] * v[0] + v[1] * v[1] + v[2] * v[2];
		return float(sqrt(s));
	}

	void MATH_CopyVector(float v1[3], float v2[3])
	{
		v1[0] = v2[0];
		v1[1] = v2[1];
		v1[2] = v2[2];
	}

	void MATH_MakeUnitVector(float v[3])
	{
		MATH_ScalarMultiplication(v, 1 / MATH_VectorMagnetude(v));
	}

	void MATH_ScalarSum(float v[3], float s)
	{
		v[0] += s;
		v[1] += s;
		v[2] += s;
	}

	float MATH_AngelBetweenTwoVectors(float v1[3], float v2[3])
	{
		return float(acos(MATH_DotProduct(v1, v2) / (MATH_VectorMagnetude(v1) * MATH_VectorMagnetude(v2))));
	}

	int MATH_factorial(int n)
	{
		int answ = 1;
		for (int i = 2; i < n + 1; i++) { answ *= i; }
		return answ;
	}

	float MATH_Power(float x, float n)
	{
		float answ = 1;
		for (int i = 0; i < n; i++) { answ *= x; }
		return answ;
	}

	float MATH_sinf(float x)
	{
		float power[4];

		power[0] = x * x * x;
		power[1] = power[0] * x * x;
		power[2] = power[1] * x * x;
		power[3] = power[2] * x * x;

		return (x - (power[0] / 6)
			+ (power[1] / 120)
			- (power[2] / 5040)
			+ (power[3] / 362880));

		/*return (x - MATH_Power(x, 3) / MATH_factorial(3)
					+ MATH_Power(x, 5) / MATH_factorial(5)
					- MATH_Power(x, 7) / MATH_factorial(7)
					+ MATH_Power(x, 9) / MATH_factorial(9));*/
	}

	//float MATH_own_cosF(float x)
	//{
		//float sin = MATH_sinf(x);
		//return sqrt(1.0 - sin * sin);
	//}
	//float MATH_own_tanF(float x)
	//{
		//float sin = MATH_sinf(x);
		//return (sin/(1 - sin*sin));
	//}

	double MATH_sin(float s) { return sin(s); }
	double MATH_log(float s) { return log(s); }
	double MATH_asin(float s) { return asin(s); }
	double MATH_cos(float s) { return cos(s); }
	double MATH_acos(float s) { return acos(s); }
	double MATH_tan(float s) { return tan(s); }
	double MATH_atan(float s) { return atan(s); }
	double MATH_sqrt(float s) { return sqrt(s); }
}
