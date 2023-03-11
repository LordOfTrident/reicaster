#include "entity.hh"

namespace ray3d {

entity::entity(float size, float max_speed):
	z(0),
	vel_z(0),
	dir(0),
	max_speed(max_speed),
	apothem(size / 2)
{}

void entity::fix_dir() {
	while (dir >= 360)
		dir -= 360;
	while (dir < 0)
		dir += 360;
}

bool entity::check_collides_in(const map &m) {
	vec2f p1 = pos + vec2f(-apothem, -apothem);
	if (m.check_point_collides(p1.x, p1.y))
		return true;

	vec2f p2 = pos + vec2f(apothem, apothem);
	if (m.check_point_collides(p2.x, p2.y))
		return true;

	vec2f p3 = pos + vec2f(apothem, -apothem);
	if (m.check_point_collides(p3.x, p3.y))
		return true;

	vec2f p4 = pos + vec2f(-apothem, apothem);
	if (m.check_point_collides(p4.x, p4.y))
		return true;

	return false;
}

void entity::update(const map &m, float friction) {
	fix_dir();

	vec2f prev = pos;

	pos.x += vel.x;
	if (check_collides_in(m))
		pos.x = prev.x;

	pos.y += vel.y;
	if (check_collides_in(m))
		pos.y = prev.y;

	z += vel_z;
	if (z < 0) {
		z     = 0;
		vel_z = 0;
	}

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
