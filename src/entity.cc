#include "entity.hh"

namespace reicaster {

entity::entity(float size, float h, float max_speed):
	z(0.9),
	vel_z(0),
	dir(0),
	max_speed(max_speed),
	apothem(size / 2),
	h(h)
{}

void entity::fix_dir() {
	while (dir >= 360)
		dir -= 360;
	while (dir < 0)
		dir += 360;
}

bool entity::check_z_collides_in(const tile &tile) {
	float z1  = z,      z2  = z + h;
	float tz1 = tile.z, tz2 = tile.z + tile.h;

	if ((z1 >= tz1 and z1 <= tz2) or (z2 >= tz1 and z2 <= tz2))
		return true;
	else
		return (tz1 >= z1 and tz1 <= z2) or (tz2 >= z1 and tz2 <= z2);
}

bool entity::check_collides_in(const map &m) {
	if (z < 0)
		return true;

	vec2f p;

	p = pos + vec2f(-apothem, -apothem);
	if (m.check_point_collides(p.x, p.y)) {
		if (check_z_collides_in(m.at(p.x, p.y)))
			return true;
	}

	p = pos + vec2f(apothem, apothem);
	if (m.check_point_collides(p.x, p.y)) {
		if (check_z_collides_in(m.at(p.x, p.y)))
			return true;
	}

	p = pos + vec2f(apothem, -apothem);
	if (m.check_point_collides(p.x, p.y)) {
		if (check_z_collides_in(m.at(p.x, p.y)))
			return true;
	}

	p = pos + vec2f(-apothem, apothem);
	if (m.check_point_collides(p.x, p.y)) {
		if (check_z_collides_in(m.at(p.x, p.y)))
			return true;
	}

	return false;
}

void entity::update(const map &m, float friction) {
	fix_dir();

	float prev_z = z;

	z += vel_z;
	if (check_collides_in(m)) {
		z     = prev_z;
		vel_z = 0;
	}

	vec2f prev = pos;

	pos.x += vel.x;
	if (check_collides_in(m))
		pos.x = prev.x;

	pos.y += vel.y;
	if (check_collides_in(m))
		pos.y = prev.y;

	vel *= friction;
}

void entity::accelerate(float amount) {
	accelerate(amount, dir);
}

void entity::accelerate(float amount, float dir) {
	vel.x += std::cos(dtor(dir)) * amount;
	vel.y += std::sin(dtor(dir)) * amount;

	auto speed = vel.mag();
	if (speed > max_speed) {
		vel.x *= max_speed / speed;
		vel.y *= max_speed / speed;
	}
}

}
