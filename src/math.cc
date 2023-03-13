#include "math.hh"

namespace reicaster {

float distance(const vec2f &a, const vec2f &b) {
	vec2f dist_vec(b - a);
	dist_vec.abs();
	return dist_vec.mag();
}

}
