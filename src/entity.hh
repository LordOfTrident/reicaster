#ifndef ENTITY_HH_HEADER_GUARD
#define ENTITY_HH_HEADER_GUARD

#include "math.hh"
#include "map.hh"

namespace reicaster {

struct entity {
	entity(float size, float max_speed = 100);

	void fix_dir();

	bool check_collides_in(const map &m);
	void update(const map &m, float friction = 1);

	void accelerate(float amount);
	void accelerate(float amount, float dir);

	vec2f pos, vel;
	float z,   vel_z;
	float dir, max_speed, apothem, h; // an 'apothem' is half of the length of a squares side
};

}

#endif
