#pragma once

#include "Vector2.h"
#include <vector>

/// <summary>
///	線形補間
/// </summary>
/// <typeparam name="type">型名</typeparam>
/// <param name="from">t=0のときの値</param>
/// <param name="to">t=1のときの値</param>
/// <param name="t">媒介変数</param>
/// <returns></returns>
template<typename type>
type Lerp(const type from, const type to, const float t);

/// <summary>
///	線形補間
/// </summary>
/// <typeparam name="type">型名</typeparam>
/// <param name="from">t=0のときの値</param>
/// <param name="to">t=1のときの値</param>
/// <param name="t">媒介変数</param>
/// <returns></returns>
template<>
Vector2 Lerp(const Vector2 from, const Vector2 to, const float t);

Vector2 Bezier(std::vector<Vector2> node, const float t);

class Easing {
public:
	class In {
	public:
		static float Sine(float t);
		static float Quad(float t);
		static float Cubic(float t);
		static float Quart(float t);
		static float Quint(float t);
		static float Expo(float t);
		static float Circ(float t);
		static float Back(float t);
		static float Elastic(float t);
		static float Bounce(float t);
	};
	class Out {
	public:
		static float Sine(float t);
		static float Quad(float t);
		static float Cubic(float t);
		static float Quart(float t);
		static float Quint(float t);
		static float Expo(float t);
		static float Circ(float t);
		static float Back(float t);
		static float Elastic(float t);
		static float Bounce(float t);
	};
	class InOut {
	public:
		static float Sine(float t);
		static float Quad(float t);
		static float Cubic(float t);
		static float Quart(float t);
		static float Quint(float t);
		static float Expo(float t);
		static float Circ(float t);
		static float Back(float t);
		static float Elastic(float t);
		static float Bounce(float t);
	};

private:

};

template<typename type>
inline type Lerp(const type from, const type to, const float t) {
	return type(from * (1 - t)) + type(to * t);
}

template<>
inline Vector2 Lerp(const Vector2 from, const Vector2 to, const float t) {
	return Vector2{ Lerp(from.x, to.x, t), Lerp(from.y, to.y, t) };
}