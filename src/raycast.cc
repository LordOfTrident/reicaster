#include "raycast.hh"

namespace ray3d {

ray_hit::ray_hit(): out_of_bounds(true), pos(-1), dist(-1), side(hit_side::left), at(nullptr) {}

// Thanks to javidx9 for the great DDA algorithm explanation video
// https://www.youtube.com/watch?v=NbSee-XM7WA
std::vector<ray_hit> cast_ray(map &m, const vec2f &from, const vec2f &dir, float max_dist) {
	std::vector<ray_hit> hits;

	float gx = dir.y / dir.x;
	float gy = dir.x / dir.y;

	vec2f step_size(std::sqrt(gx * gx + 1), std::sqrt(1 + gy * gy));

	vec2f pos(from), len, step;
	pos.floor();

	if (dir.x < 0) {
		step.x = -1;
		len.x  = (from.x - pos.x) * step_size.x;
	} else {
		step.x = 1;
		len.x  = (pos.x + 1 - from.x) * step_size.x;
	}

	if (dir.y < 0) {
		step.y = -1;
		len.y  = (from.y - pos.y) * step_size.y;
	} else {
		step.y = 1;
		len.y  = (pos.y + 1 - from.y) * step_size.y;
	}

	float dist  = 0;
	bool  horiz = false, in_wall = true;
	while (true) {
		if (m.check_point_collides(pos.x, pos.y)) {
			ray_hit hit;
			hit.dist = dist;

			if (not in_wall) {
				hit.at = &m.at(static_cast<std::size_t>(pos.x), static_cast<std::size_t>(pos.y));

				if (horiz)
					hit.side = step.x < 0? hit_side::left : hit_side::right;
				else
					hit.side = step.y < 0? hit_side::up : hit_side::down;

				if (max_dist == -1 or dist >= max_dist) {
					hit.out_of_bounds = false;
					hit.pos = from + dir * dist;

					if (horiz)
						hit.pos.x = std::round(hit.pos.x);
					else
						hit.pos.y = std::round(hit.pos.y);
				}
			}

			hits.push_back(hit);

			if (hit.at->z == 0 and hit.at->h == 1)
				break;
		}
		in_wall = false;

		if (max_dist != -1 and dist >= max_dist)
			break;

		if (len.x < len.y) {
			pos.x += step.x;
			dist   = len.x;
			len.x += step_size.x;

			horiz = true;
		} else {
			pos.y += step.y;
			dist   = len.y;
			len.y += step_size.y;

			horiz = false;
		}
	}

	return hits;
}

}
