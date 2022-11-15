#ifndef src_wfc_class_hpp
#define src_wfc_class_hpp

// src/wfc_class.hpp

#include "misc_includes.hpp"
#include "rule_class.hpp"
#include "metatile_class.hpp"

//--------
namespace wfc {
//--------
//enum Tile {
//	Land = 'L',
//	Coast = 'C',
//	Sea = 'S',
//	//Mountain = 'M',
//};
using TileUset = std::unordered_set<size_t>;
class PotElem final {
public:		// variables
	//std::unordered_set<size_t> data;
	//std::vector<std::optional<std::string>> data;
	std::vector<bool> data;
public:		// functions
	inline bool contains(size_t ti) const {
		return data.at(ti);
		//return static_cast<bool>(data.at(ti));
		//return data.contains(ti);
	}
	//inline void insert_maybe(size_t ti) {
	//	if (!contains(ti)) {
	//		insert(ti);
	//	}
	//}
	//inline void push_back(size_t i) {
	//}
	inline void insert(size_t ti) {
		data.at(ti) = true;
		//data.at(ti) = "asdf";
		//data.insert(ti);
	}
	inline void erase(size_t ti) {
		data.at(ti) = false;
		//data.at(ti) = std::nullopt;
		//data.erase(data.begin() + ti);
		//data.erase(ti);
	}
	inline size_t num_active() const {
		size_t ret = 0;
		for (const auto& ti: data) {
			if (ti) {
				++ret;
			}
		}
		return ret;
		//return data.size();
	}
	inline std::optional<size_t> first_set() const {
		//for (const auto& ti: data) {
		//	if (ti) {
		//		return ti;
		//	}
		//}
		for (size_t ti=0; ti<data.size(); ++ti) {
			if (data.at(ti)) {
				return ti;
			}
		}
		return std::nullopt;
		//return *data.begin();
	}
};
inline std::ostream& operator << (
	std::ostream& os, const PotElem& pot_elem
) {
	for (size_t ti=0; ti<pot_elem.data.size(); ++ti) {
		if (pot_elem.data.at(ti)) {
			osprintout(os, ti);
		} else {
			osprintout(os, "?");
		}
	}
	return os;
}

class Wfc final {
public:		// types
	//using Potential = std::vector<std::vector<TileUset>>;
	//using PotElem = std::pair<size_t, TileUset>;
	//class PotElem final {
	//public:		// variables
	//	//TileUset domain;
	//	std::unordered_map<size_t, bool> domain;
	//	//std::optional<size_t> sel_tile = std::nullopt;
	//};
	//class PotElemVal final {
	//public:		// variables
	//	bool
	//		visited,
	//		valid;
	//};
	//using PotElem = std::unordered_map<size_t, bool>;
	//using PotElem
	//	//= std::unordered_set<size_t>;
	//	= std::vector<std::optional<int>>;
public:		// static funcs
public:		// types
	using Potential = std::vector<std::vector<PotElem>>;
	using PotentialUmap = std::unordered_map<Vec2<size_t>, PotElem>;

	//// "Tprops" is short for "tile properties"
	//class Tprops final {
	//public:		// variables
	//	RuleUset rule_uset;
	//	double weight;
	//};
	//class Action final {
	//public:		// variables
	//	PotElem pot_elem;
	//	Vec2<size_t> pos;
	//};
	//class BaktkState final {
	//public:		// variables
	//	Vec2<size_t> pos;
	//	//Potential potential;
	//	PotElem pot_elem;
	//};
	using PosDarr = std::vector<Vec2<size_t>>;
	using Guess = std::pair<Vec2<size_t>, PotElem>;
	class BaktkStkItem final {
	public:		// variables
		Potential potential;
		PosDarr least_entropy_pos_darr;

		//std::unordered_map<Vec2<size_t>, PotElem> guess_darr;
		//PotentialUmap guess_umap;
		std::vector<Guess> guess_darr;
		size_t guess_index = size_t(-1);
		Vec2<size_t> guess_pos = Vec2<size_t>(-1, -1);
		size_t guess_ti = -1;
		//PotentialUmap guesses;
		//PosDarr least_entropy_pos_darr;

		//BaktkState prev_state;
		// states to try
		//std::vector<BaktkState> next_state_darr; 
		//std::unordered_map<Vec2<size_t>, BaktkState> next_state_umap;
		//PotentialUmap pot_umap;
	public:		// functions
		//void print_guess_umap() {
		//	for (const auto& guess: guess_umap) {
		//		printout("{", guess.first, " ",
		//			"{");
		//		for (const auto& ti: guess.second) {
		//			printout(ti, " ");
		//		}
		//		printout("}",
		//			"}\n");
		//	}
		//}
		//void print_guess_umap_at_gp() {
		//	printout("guess_umap.at(gp): ",
		//		"{");
		//	for (const auto& ti: guess_umap.at(guess_pos)) {
		//		printout(ti, " ");
		//	}
		//	printout("}\n");
		//}
		//void init_guess_umap()
		void init_guess_darr() {
			//guess_umap.clear();
			guess_darr.clear();
			//Vec2<size_t> pos;
			//for (pos.y=0; pos.y<potential.size(); ++pos.y) {
			//	const auto& row = potential.at(pos.y);
			//	for (pos.x=0; pos.x<row.size(); ++pos.x) {
			//		const auto& item = row.at(pos.x);
			//		guess_uset.insert(std::pair(pos, item));
			//	}
			//}
			for (const Vec2<size_t>& pos: least_entropy_pos_darr) {
				const auto& pot_elem = potential.at(pos.y).at(pos.x);
				//for (const auto& ti: pot_elem) {
					//guess_darr.push_back(Guess(pos, ti));
				//}
				//guess_umap.insert(std::pair(pos, pot_elem));
				guess_darr.push_back(std::pair(pos, pot_elem));
			}
		}
		void init_guess_pos() {
			//size_t i = 0;
			// Since `std::unordered_map`'s `iterator`s don't allow adding
			// arbitrary offsets to them, we use the below `for` loop.
			//for (const auto& item: guess_umap) {
			//	if (i == guess_index) {
			//		guess_pos = item.first;
			//		break;
			//	}
			//	++i;
			//}
			guess_pos = guess_darr.at(guess_index).first;
		}
		//inline const Vec2<size_t>& guess() const {
		//	return guess_darr.at(guess_index);
		//}
		//inline const Vec2<size_t>& tc_pos() const { // to collapse pos
		//	return guess().first;
		//}
		//inline const size_t& ti() const { // tile index
		//	return guess().second;
		//}
		inline void erase_guess() {
			//guess_umap.at(guess_pos).erase(guess_ti);
			//if (guess_umap.at(guess_pos).size() == 0) {
			//	guess_umap.erase(guess_pos);
			//}

			//guess_darr.at(guess_index).second.erase(guess_ti);
			guess_darr.at(guess_index).second.erase(guess_ti);
			if (guess_darr.at(guess_index).second.num_active() == 0) {
				guess_darr.erase(guess_darr.begin() + guess_index);
			}
		}
	};
private:		// variables
	Potential _result;
	//std::stack<std::vector<Potential>> _potential_darr_stk;
	std::stack<BaktkStkItem> _baktk_stk; // for backtracking
	Vec2<size_t>
		_size_2d;
	size_t 
		_mt_dim; // metatile dimension
	bool
		_backtrack,
		//_overlap;
		_rotate,
		_reflect;
	//std::unordered_map<size_t, RuleUset> _rules_umap;
	//RuleUset _rule_uset;

	std::vector<Metatile> _mt_darr;

	// This maps rules to their weights
	std::unordered_map<Rule, double> _r2w_umap;

	// This maps tiles to their weights
	//std::unordered_map<size_t, double> _weight_umap;
	std::vector<double> _weight_darr;
public:		// types
	using Rng = pcg64;
	using Ddist = std::discrete_distribution<u64>;
private:		// variables
	Rng _rng;
public:		// functions
	Wfc();
	Wfc(
		const Vec2<size_t>& s_size_2d, size_t s_mt_dim,
		const std::vector<std::vector<size_t>>& input_tiles,
		bool s_backtrack,
		//bool s_overlap,
		bool s_rotate, bool s_reflect,
		u64 s_rng_seed
	);
	GEN_CM_BOTH_CONSTRUCTORS_AND_ASSIGN(Wfc);
	~Wfc();

	GEN_GETTER_BY_CON_REF(result);
	GEN_GETTER_BY_CON_REF(size_2d);
	GEN_GETTER_BY_CON_REF(mt_dim);
	//GEN_GETTER_BY_CON_REF(rules_umap);
	//GEN_GETTER_BY_CON_REF(rule_uset);
	GEN_GETTER_BY_VAL(backtrack);
	//GEN_GETTER_BY_VAL(overlap);
	GEN_GETTER_BY_VAL(rotate);
	GEN_GETTER_BY_VAL(reflect);
	GEN_GETTER_BY_CON_REF(mt_darr);
	GEN_GETTER_BY_CON_REF(r2w_umap);
	//GEN_GETTER_BY_CON_REF(weight_umap);
	GEN_GETTER_BY_CON_REF(weight_darr);
	//GEN_GETTER_BY_CON_REF(tprops_umap);
private:		// functions
	void _learn(const std::vector<std::vector<size_t>>& input_tiles);
	void _gen();
	//void _post_process();
	//--------
	//bool _gen_outer_iteration();
	//bool _gen_inner_iteration();
	//void _gen_iteration(
	//	//Potential& potential//,
	//	//size_t depth
	//);
	//void _backtrack();
	//std::optional<Potential> _backtrack_next(
	//	//std::vector<Vec2<size_t>>& least_entropy_pos_darr,
	//	//size_t to_collapse_pos_index,
	//	const Vec2<size_t>& to_collapse_pos,
	//	//size_t pot_index
	//	const Potential& old_potential
	//);
	//bool _inner_gen_iteration();
	class CollapseTemps final {
	public:		// variables
		Vec2<size_t> pos;
		std::vector<double>
			//weight_darr;
			modded_weight_darr;
		std::vector<size_t> tile_darr;
		std::unordered_map<size_t, size_t> tid_umap;
	};
	CollapseTemps _calc_collapse_temps(
		const Potential& potential,
		//const PotentialUmap& pot_umap,
		const Vec2<size_t>& pos
	) const;
	double _calc_modded_weight(
		const Potential& potential,
		//const PotentialUmap& pot_umap,
		const Vec2<size_t>& pos, const size_t& item
	) const;
	//--------
	//std::optional<Vec2<size_t>> _rand_pos_w_least_entropy();
	std::optional<PosDarr> _calc_least_entropy_pos_darr(
		const Potential& potential
	);
	//--------
	//using Neighbor = std::pair<Dir, Vec2<size_t>>;
	class Neighbor final {
	public:		// variables
		// `d` is the incoming direction when in `_add_constraint()`
		Dir d; 
		Vec2<size_t> pos;
	};
	//--------
	void _propagate(
		Potential& potential,
		const Vec2<size_t>& start_pos
	);
	void _add_constraint(
		Potential& potential,
		const Vec2<size_t>& pos,
		const Neighbor& neighbor,
		std::queue<Vec2<size_t>>& needs_update
		//std::stack<Vec2<size_t>>& needs_update
	);
	//--------
	std::vector<Neighbor> _neighbors(
		const Vec2<size_t>& pos
	) const;
	//--------
	void _dbg_print(const BaktkStkItem& bts_item) const;
	inline void _dbg_print() const {
		_dbg_print(_baktk_stk.top());
	}
	//--------
};
//--------
} // namespace wfc

#endif		// src_wfc_class_hpp
