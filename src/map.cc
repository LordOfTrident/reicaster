#include "map.hh"

namespace reicaster {

tile::tile(): collision(false) {}
tile::tile(wall::id left, wall::id right, wall::id up, wall::id down, float h, float z):
	collision(true),
	z(z),
	h(h)
{
	id(direction::left)  = left;
	id(direction::right) = right;
	id(direction::up)    = up;
	id(direction::down)  = down;
}

tile::tile(wall::id id, float h, float z):
	ids{id, id, id, id},
	collision(true),
	z(z),
	h(h)
{}

wall::id &tile::id(direction side) {
	return ids[static_cast<int>(side)];
}

wall::id tile::id(direction side) const {
	return ids[static_cast<int>(side)];
}

static tile ascii_to_tile(char ascii) {
	switch (ascii) {
	case ' ': return tile();
	case '_': return tile(wall::bricks, 0.4);
	case '-': return tile(wall::bricks, 0.3, 0.3);
	case 'W': return tile(wall::bricks_with_paint);
	default:  return tile(wall::bricks);
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

bool map::check_point_collides(float x, float y, float z) const {
	if (static_cast<std::size_t>(x) >= w or
	    static_cast<std::size_t>(y) >= h or x < 0 or y < 0 or z < 0)
		return true;
	else if (at(x, y).collision) {
		const auto &tile = at(x, y);
		return z >= tile.z and z <= tile.z + tile.h;
	} else
		return false;
}

}
