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
	using PotElem = std::unordered_set<size_t>;
	using Potential = std::vector<std::vector<PotElem>>;

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
	//class BaktkStkItem final {
	//public:		// variables
	//	std::vector<Vec2<size_t>> least_entropy_pos_darr;

	//	//BaktkState prev_state;
	//	// states to try
	//	//std::vector<BaktkState> next_state_darr; 
	//	std::unordered_map<Vec2<size_t>, BaktkState> next_state_umap;
	//};
	//class BaktkTree final {
	//};
private:		// variables
	Potential _result;
	//std::stack<std::vector<Potential>> _potential_darr_stk;
	//std::stack<BaktkStkItem> _baktk_stk; // for backtracking
	Vec2<size_t>
		_size_2d;
	size_t 
		_mt_dim; // metatile dimension
	bool
		_no_rotate,
		_no_reflect;
		//_no_overlap;
	//std::unordered_map<size_t, RuleUset> _rules_umap;
	//RuleUset _rule_uset;

	std::vector<Metatile> _mt_darr;

	// This maps rules to their weights
	std::unordered_map<Rule, double> _r2w_umap;

	// This maps tiles to their weights
	std::unordered_map<size_t, double> _weight_umap;
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
		bool s_no_rotate, bool s_no_reflect, //bool s_no_overlap,
		u64 s_rng_seed
	);
	GEN_CM_BOTH_CONSTRUCTORS_AND_ASSIGN(Wfc);
	~Wfc();

	GEN_GETTER_BY_CON_REF(result);
	GEN_GETTER_BY_CON_REF(size_2d);
	GEN_GETTER_BY_CON_REF(mt_dim);
	//GEN_GETTER_BY_CON_REF(rules_umap);
	//GEN_GETTER_BY_CON_REF(rule_uset);
	GEN_GETTER_BY_VAL(no_rotate);
	GEN_GETTER_BY_VAL(no_reflect);
	//GEN_GETTER_BY_VAL(no_overlap);
	GEN_GETTER_BY_CON_REF(mt_darr);
	GEN_GETTER_BY_CON_REF(r2w_umap);
	GEN_GETTER_BY_CON_REF(weight_umap);
	//GEN_GETTER_BY_CON_REF(tprops_umap);
private:		// functions
	void _learn(const std::vector<std::vector<size_t>>& input_tiles);
	void _gen();
	//void _post_process();
	//--------
	//bool _gen_outer_iteration();
	//bool _gen_inner_iteration();
	bool _backtrack(
		Potential& potential//,
		//size_t depth
	);
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
		const Vec2<size_t>& pos
	) const;
	double _calc_modded_weight(
		const Potential& potential,
		const Vec2<size_t>& pos, const size_t& item
	) const;
	//--------
	//std::optional<Vec2<size_t>> _rand_pos_w_least_entropy();
	std::optional<std::vector<Vec2<size_t>>>
	_calc_least_entropy_pos_darr(
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
	);
	//--------
	std::vector<Neighbor> _neighbors(
		const Vec2<size_t>& pos
	) const;
	//--------
	//void _dbg_print() const;
	//--------
};
//--------
} // namespace wfc

#endif		// src_wfc_class_hpp
