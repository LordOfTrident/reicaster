#include "common.hh"

namespace reicaster {

error::error(): ok(true) {}
error::error(const std::string &msg): ok(false), msg(msg) {}

}
