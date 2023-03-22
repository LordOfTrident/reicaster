#ifndef RAYCAST_HH_HEADER_GUARD
#define RAYCAST_HH_HEADER_GUARD

#include <vector> // std::vector

#include "common.hh"
#include "math.hh"
#include "map.hh"

namespace reicaster {

struct ray_hit {
	ray_hit();

	bool      out_of_bounds;
	vec2f     pos;
	float     dist;
	direction side;
	tile     *at;
};

std::vector<ray_hit> cast_ray(map &m, const vec2f &from, const vec2f &dir, float max_dist = -1);

}

#endif
