#ifndef COMMON_HH_HEADER_GUARD
#define COMMON_HH_HEADER_GUARD

#include <string>  // std::string
#include <cstring> // std::strlen
#include <sstream> // std::stringstream
#include <cstdlib> // std::size_t
#include <cassert> // assert

#define CFMT(...) "%s", fmt(__VA_ARGS__).c_str()

namespace reicaster {

class error {
public:
	error();
	error(const std::string &msg);

	bool is_ok() const;
	const std::string &what() const;

private:
	bool        ok;
	std::string msg;
};

template<typename T>
class result {
public:
	result(const error &err): err(err) {}
	result(T val): val(std::move(val)) {}

	auto is_ok() const {return err.is_ok();}
	const auto &what() const {return err.what();}

	const T &get_val() const {return val;}
	T       &get_val()       {return val;}

private:
	error err;
	T     val;
};

constexpr auto fmt_seq = "%v";

template<typename... args>
std::string fmt(std::string fmt, args... a) {
	std::stringstream ss;

	([&] {
		auto start = fmt.find(fmt_seq);
		if (start == std::string::npos)
			assert(0 && "Missing format");

		if (start > 0) {
			if (fmt[start - 1] == '%')
				return;
		}

		ss << fmt.substr(0, start);
		ss << a;

		fmt = fmt.substr(start + std::strlen(fmt_seq));
	}(), ...);

	ss << fmt;
	return ss.str();
}

}

#endif
