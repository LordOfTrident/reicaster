#include "pixels.hh"

namespace reicaster {

void pixels::create(std::size_t w, std::size_t h) {
	this->w    = w;
	this->h    = h;
	this->size = w * h * sizeof(*raw);

	raw = new Uint32[w * h];
}

void pixels::destroy() {
	delete[] raw;
}

void pixels::clear() {
	std::memset(raw, 0, this->size);
}

Uint32 pixels::at(int x, int y) const {
	return raw[y * w + x];
}

Uint32 &pixels::at(int x, int y) {
	return raw[y * w + x];
}

bool pixels::check_point_bounds(int x, int y) {
	if (x < 0 or y < 0)
		return false;
	else if (static_cast<std::size_t>(x) >= w or static_cast<std::size_t>(y) >= h)
		return false;
	else
		return true;
}


}
