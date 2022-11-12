#ifndef src_metatile_class_hpp
#define src_metatile_class_hpp

// src/metatile_class.hpp

#include "misc_includes.hpp"

namespace wfc {
//--------
class Metatile final {
private:		// variables
	std::vector<std::vector<size_t>> _data;
public:		// functions
	Metatile();
	Metatile(size_t s_dim);
	GEN_CM_BOTH_CONSTRUCTORS_AND_ASSIGN(Metatile);
	~Metatile();

	const size_t& tl_corner() const {
		return data().front().front();
	}

	constexpr inline auto operator <=> (
		const Metatile& to_cmp
	) const = default;

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
	Metatile& reflect_x();
	Metatile& reflect_y();
	Metatile& transpose();
	Metatile& rotate_p90();

	GEN_GETTER_BY_CON_REF(data);
};
//--------
} // namespace wfc

namespace std {
//--------
template<>
struct hash<wfc::Metatile> {
	std::size_t operator () (const wfc::Metatile& mt) const noexcept {
		//return std::hash<decltype(mt.data())>{}(mt.data());
		size_t ret = liborangepower::containers::hash_va(mt.at({0, 0}));
		Vec2<size_t> pos;
		for (pos.y=0; pos.y<mt.dim(); ++pos.y) {
			for (pos.x=0; pos.x<mt.dim(); ++pos.x) {
				if (pos != Vec2<size_t>(0, 0)) {
					ret = liborangepower::containers::hash_merge
						(ret, mt.at(pos));
				}
			}
		}
		return ret;
	}
};
//--------
}

#endif		// src_metatile_class_hpp
