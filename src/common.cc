#include "common.hh"

namespace ray3d {

error::error(): ok(true) {}
error::error(const std::string &msg): ok(false), msg(msg) {}

}
