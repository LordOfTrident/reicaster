#ifndef MATH_HH_HEADER_GUARD
#define MATH_HH_HEADER_GUARD

#include <string>  // std::string
#include <ostream> // std::ostream
#include <cmath>   // std::sqrt, std::abs, std::round, std::floor, std::ceil, std::cos, std::sin

#ifndef M_PI
    #define M_PI 3.14159265358979323846
#endif

#include "common.hh"

namespace reicaster {

constexpr float dtor(float deg) {
	return deg * M_PI / 180;
}

constexpr float rtod(float rad) {
	return rad * 180 / M_PI;
}

template<typename T>
struct vec2 {
	static vec2<T> from_rad(float rad) {
		return vec2(std::cos(rad), std::sin(rad));
	}

	static vec2<T> from_deg(float deg) {
		return vec2(std::cos(dtor(deg)), std::sin(dtor(deg)));
	}

	vec2():         x(0), y(0) {}
	vec2(T x, T y): x(x), y(y) {}
	vec2(T a):      x(a), y(a) {}

#define RAY3D_VEC2_MAKE_ARITH_OP(OP) \
	vec2 operator OP(const vec2 &a) const { \
		return vec2(x OP a.x, y OP a.y); \
	}

#define RAY3D_VEC2_MAKE_ARITH_ASSIGN_OP(OP) \
	vec2 operator OP(const vec2 &a) { \
		x OP a.x; \
		y OP a.y; \
		return *this; \
	}

	RAY3D_VEC2_MAKE_ARITH_OP(+)
	RAY3D_VEC2_MAKE_ARITH_OP(-)
	RAY3D_VEC2_MAKE_ARITH_OP(*)
	RAY3D_VEC2_MAKE_ARITH_OP(/)

	RAY3D_VEC2_MAKE_ARITH_ASSIGN_OP(+=)
	RAY3D_VEC2_MAKE_ARITH_ASSIGN_OP(-=)
	RAY3D_VEC2_MAKE_ARITH_ASSIGN_OP(*=)
	RAY3D_VEC2_MAKE_ARITH_ASSIGN_OP(/=)

	bool operator ==(const vec2 &a) const {
		return x == a.x and y == a.y;
	}

	bool operator !=(const vec2 &a) const {
		return not (*this == a);
	}

	void norm() {
		*this /= vec2(mag());
	}

	#define RAY3D_VEC2_MAKE_SIMPLE_MODIFIER(NAME, FUNC) \
		void NAME() { \
			x = FUNC(x); \
			y = FUNC(y); \
		}

	RAY3D_VEC2_MAKE_SIMPLE_MODIFIER(abs,   std::abs)
	RAY3D_VEC2_MAKE_SIMPLE_MODIFIER(round, std::round)
	RAY3D_VEC2_MAKE_SIMPLE_MODIFIER(floor, std::floor)
	RAY3D_VEC2_MAKE_SIMPLE_MODIFIER(ceil,  std::ceil)

	T mag() const {
		return std::sqrt(x * x + y * y);
	}

	std::string stringify() const {
		return fmt("[%v; %v]", x, y);
	}

	friend std::ostream &operator <<(std::ostream &stream, const vec2 &a) {
		stream << a.stringify();
	    return stream;
	}

	T x, y;
};

using vec2f = vec2<float>;
using vec2i = vec2<int>;

float distance(const vec2f &a, const vec2f &b);

}

#endif
