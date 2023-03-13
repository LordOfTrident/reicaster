#include "map.hh"

namespace reicaster {

tile::tile(): collision(false) {}
tile::tile(wall_type left, wall_type right, wall_type up, wall_type down, float h, float z):
	left(left),
	right(right),
	up(up),
	down(down),
	collision(true),
	z(z),
	h(h)
{}

tile::tile(wall_type type, float h, float z):
	left(type),
	right(type),
	up(type),
	down(type),
	collision(true),
	z(z),
	h(h)
{}

static tile ascii_to_tile(char ascii) {
	switch (ascii) {
	case ' ': return tile();
	case '_': return tile(wall_type::bricks, 0.4);
	case '-': return tile(wall_type::bricks, 0.3, 0.3);
	default:  return tile(wall_type::bricks);
	}
}

void map::create(std::size_t w, std::size_t h, const char *ascii_representation) {
	this->w    = w;
	this->h    = h;
	this->size = w * h * sizeof(*raw);

	raw = new tile[w * h];

	size_t len = w * h;
	for (std::size_t i = 0;; ++ i) {
		bool ascii_end = ascii_representation[i] == '\0';
		bool raw_end   = i >= len;

		if (ascii_end && raw_end)
			break;
		else if (ascii_end)
			assert(0 && "map ascii representation is too short");
		else if (raw_end)
			assert(0 && "map ascii representation is too long");

		raw[i] = ascii_to_tile(ascii_representation[i]);
	}
}

void map::destroy() {
	delete[] raw;
}

tile map::at(std::size_t x, std::size_t y) const {
	return raw[(y * w) + x];
}

tile &map::at(std::size_t x, std::size_t y) {
	return raw[(y * w) + x];
}

bool map::check_point_collides(float x, float y) const {
	if (static_cast<std::size_t>(x) >= w or static_cast<std::size_t>(y) >= h or x < 0 or y < 0)
		return true;
	else
		return at(x, y).collision;
}

}
