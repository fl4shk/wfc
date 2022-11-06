#ifndef src_rule_class_hpp
#define src_rule_class_hpp

// src/rule_class.hpp

#include "misc_includes.hpp"

namespace wfc {
//--------
enum class Dir: u32 {
	Left = 0,
	Top = 1,
	Right = 2,
	Bottom = 3,
	Bad = 4,
};
constexpr inline Dir reverse(Dir d) {
	switch (d) {
	//--------
	case Dir::Left:
		return Dir::Right;
	case Dir::Top:
		return Dir::Bottom;
	case Dir::Right:
		return Dir::Left;
	case Dir::Bottom:
		return Dir::Top;
	default:
		return Dir::Bad;
	//--------
	}
}
constexpr inline std::ostream& operator << (
	std::ostream& os, const Dir& d
) {
	switch (d) {
	//--------
	case Dir::Left:
		return osprintout(os, "Dir::Left");
	case Dir::Top:
		return osprintout(os, "Dir::Top");
	case Dir::Right:
		return osprintout(os, "Dir::Right");
	case Dir::Bottom:
		return osprintout(os, "Dir::Bottom");
	default:
		return osprintout(os, "Dir::Bad");
	//--------
	}
}

class Rule final {
public:		// variables
	u32 t0, t1; // tiles
	Dir d; // direction
public:		// functions
	constexpr inline auto operator <=> (
		const Rule& to_cmp
	) const = default;
	constexpr inline Rule reverse() const {
		Rule ret = *this;
		std::swap(ret.t0, ret.t1);
		ret.d = wfc::reverse(ret.d);
		//return Rule{.t0=t1, .t1=t0, .d=::reverse(d)};
		return ret;
	}
};
using RuleUset = std::unordered_set<Rule>;
constexpr inline std::ostream& operator << (
	std::ostream& os, const Rule& rule
) {
	os
		<< "{" << "\'" << rule.t0 << "\' << "
		<< "\'" << rule.t1 << "\' << "
		<< rule.d << "}";
	return os;
}
//--------
} // namespace wfc

namespace std {
//--------
template<>
struct hash<wfc::Rule> {
	std::size_t operator () (const wfc::Rule& key) const noexcept {
		return hash_va(key.t0, key.t1, key.d);
	}
};
//--------
} // namespace std

#endif		// src_rule_class_hpp
