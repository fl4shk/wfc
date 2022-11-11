#ifndef src_rule_class_hpp
#define src_rule_class_hpp

// src/rule_class.hpp

#include "misc_includes.hpp"

namespace wfc {
//--------
// Direction
enum class Dir: u32 {
	Left = 0,
	Top = 1,
	Right = 2,
	Bottom = 3,
	//Bad = 4,
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
		//return Dir::Bad;
		throw std::invalid_argument(sconcat
			("wfc::reverse(): Error: ",
			"unknown `Dir` ", u32(d), "."));
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
		//return osprintout(os, "Dir::Bad");
		throw std::invalid_argument(sconcat
			("wfc::operator << (std::ostream&, const Dir&): Error: ",
			"unknown `Dir` ", u32(d), "."));
	//--------
	}
}

// Rotation
enum class Rot: u32 {
	Deg0 = 0,
	Deg90 = 1,
	Deg180 = 2,
	Deg270 = 3,
	//Bad = 4,
};

constexpr inline std::ostream& operator << (
	std::ostream& os, const Rot& r
) {
	switch (r) {
	//--------
	case Rot::Deg0:
		return osprintout(os, "Rot::Deg0");
	case Rot::Deg90:
		return osprintout(os, "Rot::Deg90");
	case Rot::Deg180:
		return osprintout(os, "Rot::Deg180");
	case Rot::Deg270:
		return osprintout(os, "Rot::Deg270");
	default:
		//return osprintout(os, "Rot::Bad");
		throw std::invalid_argument(sconcat
			("wfc::operator << (std::ostream&, const Rot&): Error: ",
			"unknown `Rot` ", u32(r), "."));
	//--------
	}
}

class Metatile final {
private:		// variables
	std::vector<std::vector<size_t>> _data;
public:		// functions
	Metatile(size_t s_dim);
	GEN_CM_BOTH_CONSTRUCTORS_AND_ASSIGN(Metatile);
	~Metatile();

	inline size_t dim() const {
		return data().size();
	}
	inline Vec2<size_t> size_2d() const {
		//return {.x=data().front().size(), .y=data().size()};
		return {.x=dim(), .y=dim()};
	}
	inline size_t& at(const Vec2<size_t>& pos) {
		return _data.at(pos.y).at(pos.x);
	}
	inline const size_t& at(const Vec2<size_t>& pos) const {
		return _data.at(pos.y).at(pos.x);
	}

	// rotate plus 90 degrees
	Metatile& rotate_p90();

	GEN_GETTER_BY_CON_REF(data);
};

class Rule final {
public:		// variables
	size_t t0, t1; // tiles
	//Metatile t0, t1;
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
	//os
	//	<< "{" << "\'" << rule.t0 << "\', "
	//		<< "\'" << rule.t1 << "\', "
	//		<< rule.d << "}";
	return osprintout(os,
		"{", "\'", rule.t0, "\', ",
			"\'", rule.t1, "\', ",
			rule.d, "}");
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
