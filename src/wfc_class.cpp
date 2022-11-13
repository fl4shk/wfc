#include "wfc_class.hpp"
#include <cmath>
//#include <algorithm>
//#include <unistd.h>

namespace wfc {
//--------
Wfc::Wfc() {}
Wfc::Wfc(
	const Vec2<size_t>& s_size_2d, size_t s_mt_dim,
	const std::vector<std::vector<size_t>>& input_tiles,
	bool s_no_rotate, bool s_no_reflect, //bool s_no_overlap,
	u64 s_rng_seed
) 
	: _size_2d(s_size_2d),
	_mt_dim(s_mt_dim),
	//_potential(s_size_2d.y,
	//	std::vector<TileUset>(s_size_2d.x, TileUset())),
	_no_rotate(s_no_rotate),
	_no_reflect(s_no_reflect),
	//_no_overlap(s_no_overlap),
	_rng(s_rng_seed) {
	//--------
	if (mt_dim() > size_2d().x) {
		throw std::invalid_argument(sconcat
			("Wfc::Wfc(): Error: ",
			"_mt_dim (", mt_dim(), ") is greater than ",
			"_size_2d.x (", size_2d().x, ")."));
	}
	if (mt_dim() > size_2d().y) {
		throw std::invalid_argument(sconcat
			("Wfc::Wfc(): Error: ",
			"_mt_dim (", mt_dim(), ") is greater than ",
			"_size_2d.y (", size_2d().y, ")."));
	}
	_learn(input_tiles);
	_gen();
	//--------
}
Wfc::~Wfc() {}

void Wfc::_learn(const std::vector<std::vector<size_t>>& input_tiles) {
	//--------
	//_tprops_umap.clear();
	//Potential potential;
	_result.clear();
	//_potential.clear();
	//_rules_umap.clear();
	//_rule_uset.clear();
	_mt_darr.clear();
	_r2w_umap.clear();
	_weight_umap.clear();

	PotElem pot_elem;

	// Insert metatiles
	if (std::unordered_set<Metatile> mt_uset; true) {
		auto traverse = [&](size_t j, size_t i) -> Metatile {
			Metatile ret(mt_dim());

			Vec2<size_t>
				loc_pos, pos;
			for (loc_pos.y=0; loc_pos.y<mt_dim(); ++loc_pos.y) {
				for (loc_pos.x=0; loc_pos.x<mt_dim(); ++loc_pos.x) {
					pos.y = (loc_pos.y + j) % input_tiles.size();
					pos.x = (loc_pos.x + i) % input_tiles.at(j).size();
					ret.at(loc_pos) = input_tiles.at(pos.y).at(pos.x);
				}
			}

			return ret;
		};

		for (size_t j=0; j<input_tiles.size(); ++j) {
			const auto& row = input_tiles.at(j);
			for (size_t i=0; i<row.size(); ++i) {
				mt_uset.insert(traverse(j, i));
			}
		}

		if (std::unordered_set<Metatile> ext_mt_uset; true) {
			for (const auto& item: mt_uset) {
				if (!no_reflect()) {
					ext_mt_uset.insert(Metatile(item).reflect_x());
					ext_mt_uset.insert(Metatile(item).reflect_y());
					ext_mt_uset.insert
						(Metatile(item).reflect_x().reflect_y());
				}
				if (!no_rotate()) {
					auto rot90 = Metatile(item).rotate_p90();
					ext_mt_uset.insert(rot90);
					if (!no_reflect()) {
						ext_mt_uset.insert(Metatile(rot90).reflect_x());
						ext_mt_uset.insert(Metatile(rot90).reflect_y());
						ext_mt_uset.insert
							(Metatile(rot90).reflect_x().reflect_y());
					}

					auto rot180 = Metatile(rot90).rotate_p90();
					ext_mt_uset.insert(rot180);
					if (!no_reflect()) {
						ext_mt_uset.insert(Metatile(rot180).reflect_x());
						ext_mt_uset.insert(Metatile(rot180).reflect_y());
						ext_mt_uset.insert
							(Metatile(rot180).reflect_x().reflect_y());
					}

					auto rot270 = Metatile(rot180).rotate_p90();
					ext_mt_uset.insert(rot270);
					if (!no_reflect()) {
						ext_mt_uset.insert(Metatile(rot270).reflect_x());
						ext_mt_uset.insert(Metatile(rot270).reflect_y());
						ext_mt_uset.insert
							(Metatile(rot270).reflect_x().reflect_y());
					}
				}
			}
			mt_uset.merge(std::move(ext_mt_uset));
		}

		for (const auto& item: mt_uset) {
			_mt_darr.push_back(item);
		}
	}

	// Insert weights
	auto insert_weight = [&](const size_t& item) -> void {
		// Insert weights, so that tiles that appear more often in
		// `input_tiles` have a higher weight
		//const size_t& item = row.at(i);
		pot_elem.insert(item);
		if (weight_umap().contains(item)) {
			//_weight_umap.at(item) += 3.0;
			_weight_umap.at(item) += 1.0;
		} else { // if (!weight_umap().contains(item))
			_weight_umap.insert(std::pair(item, 1.0));
		}
	};
	//for (const auto& item: mt_darr()) 
	for (size_t i=0; i<mt_darr().size(); ++i) {
		insert_weight(i);
	}
	//printout("Inserting weights\n");
	//for (i32 j=0; j<i32(input_tiles.size()); ++j) {
	//	const auto& row = input_tiles.at(j);
	//	//printout(row, "\n");
	//	for (i32 i=0; i<i32(row.size()); ++i) {
	//		insert_weight(row.at(i);
	//	}
	//}
	_result = Potential(size_2d().y,
		std::vector<PotElem>(size_2d().x, pot_elem));
	//_potential_darr_stk.push({std::move(potential)});

	// Insert `Rule`s
	auto insert_rule = [&](const Rule& rule) -> void {
		//printout("in insert_rule():\n");
		//if (!rule_uset().contains(rule))
		if (!r2w_umap().contains(rule)) {
			//_rule_uset.insert(rule);
			_r2w_umap.insert(std::pair(rule, 1.0));
		} else { // if (r2w_umap().contains(rule))
			_r2w_umap.at(rule) += 1.0;
		}
	};
	auto mt_compatible = [this](size_t j, size_t i, Dir d) -> bool {
		const Metatile
			& item_j = mt_darr().at(j),
			& item_i = mt_darr().at(i);

		Vec2<size_t>
			offset_j{0, 0},
			offset_i{0, 0},
			offset_size_2d{0, 0};

		switch (d) {
		//--------
		case Dir::Left:
			offset_i = offset_size_2d = {1, 0};
			break;
		case Dir::Top:
			offset_i = offset_size_2d = {0, 1};
			break;
		case Dir::Right:
			offset_j = offset_size_2d = {1, 0};
			break;
		case Dir::Bottom:
			offset_j = offset_size_2d = {0, 1};
			break;
		default:
			break;
		//--------
		}

		Vec2<size_t> loc_pos;
		for (
			loc_pos.y=0;
			loc_pos.y<(mt_dim() - offset_size_2d.y);
			++loc_pos.y
		) {
			for (
				loc_pos.x=0;
				loc_pos.x<(mt_dim() - offset_size_2d.x);
				++loc_pos.x
			) {
				const Vec2<size_t>
					pos_j = loc_pos + offset_j,
					pos_i = loc_pos + offset_i;

				if (item_j.at(pos_j) != item_i.at(pos_i)) {
					return false;
				}
			}
		}

		return true;
	};
	for (size_t j=0; j<mt_darr().size(); ++j) {
		for (size_t i=0; i<mt_darr().size(); ++i) {
			if (mt_compatible(j, i, Dir::Left)) {
				insert_rule(Rule{.t0=j, .t1=i, .d=Dir::Left});
			}
			if (mt_compatible(j, i, Dir::Top)) {
				insert_rule(Rule{.t0=j, .t1=i, .d=Dir::Top});
			}
			if (mt_compatible(j, i, Dir::Right)) {
				insert_rule(Rule{.t0=j, .t1=i, .d=Dir::Right});
			}
			if (mt_compatible(j, i, Dir::Bottom)) {
				insert_rule(Rule{.t0=j, .t1=i, .d=Dir::Bottom});
			}
		}
	}
	//for (i32 j=0; j<i32(input_tiles.size()); ++j) {
	//	const auto& row = input_tiles.at(j);
	//	for (i32 i=0; i<i32(row.size()); ++i) {
	//		const auto& item = row.at(i);
	//		//auto get_metatile = [&]
	//		auto insert_rule = [&](const Rule& rule) -> void {
	//			//printout("in insert_rule():\n");
	//			//if (!rule_uset().contains(rule))
	//			if (!r2w_umap().contains(rule)) {
	//				//_rule_uset.insert(rule);
	//				_r2w_umap.insert(std::pair(rule, 1.0));
	//			} else { // if (r2w_umap().contains(rule))
	//				_r2w_umap.at(rule) += 1.0;
	//			}
	//		};
	//		//// left
	//		//if (i > 0) {
	//		//	//printout("i > 0: ");
	//		//	const Rule rule{item, row.at(i - 1), Dir::Left};
	//		//	//const Rule rule{row.at(i - 1), item, Dir::Left};
	//		//	insert_rule(rule);
	//		//	insert_rule(rule.reverse());
	//		//}
	//		//// top
	//		//if (j > 0) {
	//		//	//printout("j > 0: ");
	//		//	const Rule rule{item, input_tiles.at(j - 1).at(i), 
	//		//		Dir::Top};
	//		//	insert_rule(rule);
	//		//	insert_rule(rule.reverse());
	//		//}
	//		//// right
	//		//if (i + 1 != i32(row.size())) {
	//		//	//printout("i + 1 != i32(row.size()): ");
	//		//	const Rule rule{item, row.at(i + 1), Dir::Right};
	//		//	insert_rule(rule);
	//		//	insert_rule(rule.reverse());
	//		//}
	//		//// bottom
	//		//if (j + 1 != i32(input_tiles.size())) {
	//		//	//printout("j + 1 != i32(input_tiles.size()): ");
	//		//	const Rule rule{item, input_tiles.at(j + 1).at(i),
	//		//		Dir::Bottom};
	//		//	insert_rule(rule);
	//		//	insert_rule(rule.reverse());
	//		//}
	//	}
	//}
	//--------
}
void Wfc::_gen() {
	//do
	//{
	//	printout("_gen(): Here's the current state:\n");
	//	_dbg_print();
	//	//printout("\n");
	//} while (_gen_iteration());

	//for (;;) {
	//	BaktkStkItem to_push;
	//	to_push.least_entropy_pos_darr = _calc_least_entropy_pos_darr();
	//	if (!to_push.least_entropy_pos_darr) {
	//		break;
	//	}

	//	_baktk_stk.push(std::move(to_push));
	//	const size_t size = _baktk_stk.size();
	//	//while (_baktk_stk.size() == size)
	//	//if (_gen_iteration()) {
	//	//	//_baktk_stk.pop();
	//	//}
	//};
	//_gen_iteration(_result);

	//while (_gen_iteration()) {
	//}
	//if (BaktkStkItem to_push{.potential=_result}; true) {
	//	auto temp_le_pos_darr
	//		= _calc_least_entropy_pos_darr(to_push.potential);
	//	_baktk_stk.push(std::move(to_push));
	//}
	//std::optional<PosDarr> least_entropy_pos_darr = std::nullopt;
	_baktk_stk.push(BaktkStkItem
		{.potential=_result});
	//_baktk_stk.top().least_entropy_pos_darr
	//	= *_calc_least_entropy_pos_darr(_baktk_stk.top().potential);
	//_baktk_stk.top().init_guess_umap();

	//auto do_pop = [&]() -> void {
	//	const size_t& guess_index = _baktk_stk.top().guess_index;
	//};
	bool did_pop = false;
	for (;;) {
		//stk_top.least_entropy_pos_darr
		//	= std::move(*temp_least_entropy_pos_darr);
		//stk_top.init_guess_umap();

		//if (_baktk_stk.top().guess_umap.size() == 0) {
		//	_baktk_stk.pop();
		//	continue;
		//}

		BaktkStkItem& stk_top = _baktk_stk.top();

		//const PosDarr& least_entropy_pos_darr
		//	//= _calc_least_entropy_pos_darr(potential);
		//	= stk_top.least_entropy_pos_darr;
		Potential& potential = stk_top.potential;
		//if (!did_pop) {
			if (
				auto temp_least_entropy_pos_darr
					= _calc_least_entropy_pos_darr(potential);
				temp_least_entropy_pos_darr
			) {
				stk_top.least_entropy_pos_darr
					= *temp_least_entropy_pos_darr;
				stk_top.init_guess_umap();
			} else {
				// At this point, we are done.
				_result = std::move(potential);
				break;
			}
		//}

		auto& guess_umap = stk_top.guess_umap;
		size_t& guess_index = stk_top.guess_index;
		Vec2<size_t>& guess_pos = stk_top.guess_pos;
		size_t& guess_ti = stk_top.guess_ti;

		//auto do_pop = [this]() -> void {
		//	_baktk_stk.pop();
		//	auto& top = _baktk_stk.top();
		//	top.guess_darr.erase(top.guess_index);
		//};
		if (did_pop) {
			did_pop = false;
		}

		guess_index = rng_run<size_t>(_rng,
			size_t(0), guess_umap.size() - 1);
		stk_top.init_guess_pos();

		if (guess_umap.at(guess_pos).size() == 0) {
			guess_umap.erase(guess_pos);
			printout("testificate 4\n");
			//stk_top.erase_guess();
			//continue;
			if (guess_umap.size() == 0) {
				_baktk_stk.pop();
				did_pop = true;
				printout("testificate 3\n");
				continue;
			}
		}
		//--------
		const auto& prev_to_collapse
			= potential.at(guess_pos.y).at(guess_pos.x);

		if (prev_to_collapse.size() == 0) {
			_baktk_stk.pop();
			did_pop = true;
			//_baktk_stk.top().erase_guess();
			//if (_baktk_stk.top().guess_umap.size() == 0) {
			//	_baktk_stk.pop();
			//}
			printout("testificate\n");
			continue;
		}

		//printout("guess_umap right before stuff with `to_push`:\n");
		//stk_top.print_guess_umap();
		//printout("guess_pos: ", guess_pos, "\n");
		//printout("guess_umap.at(gp).size(): ",
		//	guess_umap.at(guess_pos).size(),
		//	"\n");

		//if (guess_umap.at(guess_pos).size() > 0) {
		//	stk_top.print_guess_umap_at_gp();
		//}
		//_dbg_print();

		BaktkStkItem to_push
			= {.potential=stk_top.potential};
		auto& to_collapse
			= to_push.potential.at(guess_pos.y).at(guess_pos.x);
		const CollapseTemps& ct
			//= _calc_collapse_temps(guess_umap, guess_pos);
			= _calc_collapse_temps(to_push.potential, guess_pos);
		Ddist ddist(ct.modded_weight_darr.begin(),
			ct.modded_weight_darr.end());
		to_collapse.clear();

		const auto& rng_val = ddist(_rng);
		guess_ti = ct.tile_darr.at(rng_val);
		to_collapse.insert(guess_ti);

		try {
			_propagate(to_push.potential, guess_pos);
		} catch (const std::exception& e) {
			did_pop = true;
			// We failed to `_propagate()`.
			_baktk_stk.pop();
			_baktk_stk.top().erase_guess();
			printout("testificate 2\n");

			continue;
		}
		_baktk_stk.push(std::move(to_push));
		//--------
	}

	//_post_process();
}

//void Wfc::_post_process() {
//}

//void Wfc::_gen_iteration(
//	//Potential& potential//,
//	//size_t depth
//) {
//	Potential& potential = _baktk_stk.top().potential;
//	const PosDarr& least_entropy_pos_darr
//		//= _calc_least_entropy_pos_darr(potential);
//		= _baktk_stk.top().least_entropy_pos_darr;
//
//	//if (!least_entropy_pos_darr) {
//	//	//printout("test 2\n");
//	//	return false;
//	//}
//
//	//std::unordered_map<Vec2<size_t>, PotElem> temp_tile_umap;
//	//PotentialUmap& temp_tile_umap;
//	//for (const auto& pos: *least_entropy_pos_darr) {
//	//	temp_tile_umap.insert(std::pair
//	//		(pos, potential.at(pos.y).at(pos.x)));
//	//}
//	//std::vector<std::pair<Vec2<size_t>, size_t>> tried_tile_darr;
//
//	BaktkStkItem bts_item;
//	Vec2<size_t> tc_pos;
//	size_t ti; // tile index
//
//	class NextRet final {
//	public:		// variables
//		Potential pot;
//		Vec2<size_t> tc_pos; // to collapse pos
//		size_t
//			ti; // tile index
//			//le_darr_index;
//		bool
//			fail_0: 1 = false,
//			fail_1: 1 = false;
//	};
//	auto next = [&](NextRet& ret) -> void {
//		//NextRet ret
//		//	{.pot=potential};
//
//		//const size_t temp_pos_index 
//		//	= rng_run<size_t>(_rng,
//		//		size_t(0), temp_tile_umap.size() - 1);
//		//if (size_t i=0; true) {
//		//	// Since `std::unordered_map`'s `iterator`s don't allow adding
//		//	// arbitrary offsets to them, we use the below `for` loop.
//		//	for (const auto& item: temp_tile_umap) {
//		//		if (i == temp_pos_index) {
//		//			ret.tc_pos = item.first;
//		//			break;
//		//		}
//		//		++i;
//		//	}
//		//}
//
//		auto& to_collapse
//			= ret.pot.at(ret.tc_pos.y).at(ret.tc_pos.x);
//
//		if (to_collapse.size() == 0) {
//			//return std::nullopt;
//			ret.fail_0 = true;
//			//return ret;
//		}
//
//		const CollapseTemps ct = _calc_collapse_temps
//			(temp_tile_umap, ret.tc_pos);
//		//Ddist ddist(ct.weight_darr.begin(), ct.weight_darr.end());
//		Ddist ddist(ct.modded_weight_darr.begin(),
//			ct.modded_weight_darr.end());
//		to_collapse.clear();
//
//		const auto& rng_val = ddist(_rng);
//		ret.ti = ct.tile_darr.at(rng_val);
//		to_collapse.insert(ret.ti);
//
//		// pretty sure this is correct
//		try {
//			_propagate(ret.pot, ret.tc_pos);
//		} catch (const std::exception& e) {
//			//return std::nullopt;
//			ret.fail_1 = true;
//		}
//		//return ret;
//	};
//
//	//NextRet nr;
//	//while (temp_tile_umap.size() > 0) {
//	//	nr.pot = potential;
//	//	_backtrack(nr.pot);
//	//	nr = next();
//	//	//if (nr.fail_0) {
//	//	//	//return false;
//	//	//} else if (nr.fail_1) {
//	//	//	temp_tile_umap.at(nr.tc_pos).erase(nr.ti);
//	//	//	if (temp_tile_umap.at(nr.tc_pos).size() == 0) {
//	//	//		temp_tile_umap.erase(nr.tc_pos);
//	//	//	}
//	//	//	//_backtrack(nr.pot);
//	//	//} else {
//	//	//	potential = std::move(nr.pot);
//	//	//	return true;
//	//	//}
//	//}
//	//return false;
//
//	//NextRet nr
//	//	{.pot=potential};
//	//auto erase_maybe = [&]() -> void {
//	//	if (nr.fail_0) {
//	//		temp_tile_umap.erase(nr.tc_pos);
//	//	} else if (nr.fail_1) {
//	//		temp_tile_umap.at(nr.tc_pos).erase(nr.ti);
//	//		if (temp_tile_umap.at(nr.tc_pos).size() == 0) {
//	//			temp_tile_umap.erase(nr.tc_pos);
//	//		}
//	//	}
//	//};
//	////next(nr);
//	////erase_maybe();
//	//while (temp_tile_umap.size() > 0) {
//	//	next(nr);
//	//	erase_maybe();
//	//}
//	//printout("testificate\n");
//}

//auto Wfc::_backtrack_next(
//	//std::vector<Vec2<size_t>>& least_entropy_pos_darr,
//	//size_t to_collapse_pos_index,
//	const Vec2<size_t>& to_collapse_pos,
//	//size_t pot_index
//	Potential& old_potential
//) -> std::optional<Potential> {
//	//auto to_collapse_pos = _rand_pos_w_least_entropy();
//	//Potential potential(_potential_darr_stk.top().at(pot_index));
//	Potential potential(old_potential);
//
//	auto& to_collapse
//		= potential.at(to_collapse_pos.y).at(to_collapse_pos.x);
//
//	// The below commented-out `if` statement should never occur
//	// because we do this check in `_rand_pos_w_least_entropy()`.
//	if (to_collapse.size() == 0) {
//		return std::nullopt;
//	}
//
//	const CollapseTemps ct = _calc_collapse_temps
//		(potential, to_collapse_pos);
//	//Ddist ddist(ct.weight_darr.begin(), ct.weight_darr.end());
//	Ddist ddist(ct.modded_weight_darr.begin(),
//		ct.modded_weight_darr.end());
//	to_collapse.clear();
//
//	const auto& rng_val = ddist(_rng);
//	to_collapse.insert(ct.tile_darr.at(rng_val));
//
//	try {
//		_propagate(potential, to_collapse_pos);
//		return potential;
//	} catch (const std::exception& e) {
//		return std::nullopt;
//	}
//
//	//return true;
//}

//bool Wfc::_gen_inner_iteration() {
//}
//bool Wfc::_inner_gen_iteration() {
//}
auto Wfc::_calc_collapse_temps(
	const Potential& potential,
	//const PotentialUmap& pot_umap,
	const Vec2<size_t>& pos
) const -> CollapseTemps {
	//WeightsAndTileIds ret;
	CollapseTemps ret;
	ret.pos = pos;
	const PotElem& pot_elem = potential.at(pos.y).at(pos.x);
	//const PotElem& pot_elem = pot_umap.at(pos);

	//std::vector<double> weight_darr;
	//std::unordered_map<size_t, size_t> tid_umap;
	if (size_t i=0; true) {
		for (const auto& item: pot_elem) {
			//ret.weight_darr.push_back(weight_umap().at(item.first));
			//ret.tile_darr.push_back(item.first);
			//ret.tid_umap.insert(std::pair(item.first, i));
			//ret.weight_darr.push_back(weight_umap().at(item));
			ret.modded_weight_darr.push_back(_calc_modded_weight
				(potential, pos, item));
			//ret.modded_weight_darr.push_back(_calc_modded_weight
			//	(pot_umap, pos, item));
			ret.tile_darr.push_back(item);
			ret.tid_umap.insert(std::pair(item, i));
			++i;
		}
	}

	return ret;
}
double Wfc::_calc_modded_weight(
	const Potential& potential,
	//const PotentialUmap& pot_umap,
	const Vec2<size_t>& pos, const size_t& item
) const {
	double ret = 0.0;

	const std::vector<Neighbor>& neighbors(_neighbors(pos));

	bool found = false;
	// If any tiles have been fully decided, we can add to this tile's
	// weight based on `_r2w_umap`.
	for (const auto& r2w_pair: r2w_umap()) {
		if (r2w_pair.first.t0 == item) {
			for (const Neighbor& neighbor: neighbors) {
				const PotElem& nb_pot_elem
					= potential.at(neighbor.pos.y).at(neighbor.pos.x);
					//= pot_umap.at(neighbor.pos);
				if (
					nb_pot_elem.size() == 1
					&& r2w_pair.first.t1 == *nb_pot_elem.begin()
					&& r2w_pair.first.d == neighbor.d
				) {
					found = true;
					ret += r2w_pair.second;
					break;
				}
			}
		} else if (r2w_pair.first.t1 == item) {
			for (const Neighbor& neighbor: neighbors) {
				const PotElem& nb_pot_elem
					= potential.at(neighbor.pos.y).at(neighbor.pos.x);
					//= pot_umap.at(neighbor.pos);
				if (
					nb_pot_elem.size() == 1
					&& r2w_pair.first.t0 == *nb_pot_elem.begin()
					&& r2w_pair.first.d == neighbor.d
				) {
					found = true;
					ret += r2w_pair.second;
					break;
				}
			}
		}
	}
	if (!found) {
		ret = weight_umap().at(item);
	}
	return ret;
}
auto Wfc::_calc_least_entropy_pos_darr(
	const Potential& potential
) -> std::optional<PosDarr> {
	std::optional<double> entropy = std::nullopt;
	PosDarr least_entropy_pos_darr;

	Vec2<size_t> pos;
	//printout("Wfc::_calc_least_entropy_pos_darr(): computing...\n");
	for (pos.y=0; pos.y<size_2d().y; ++pos.y) {
		for (pos.x=0; pos.x<size_2d().x; ++pos.x) {
			const auto& tiles = potential.at(pos.y).at(pos.x);
			//const CollapseTemps ct = _calc_collapse_temps(pos);
			//if (ct.tile_darr.size() == 0)
			if (tiles.size() == 0) {
				// We've failed the generation at this point.
				//_dbg_print();
				//sleep(1);
				//throw std::runtime_error(sconcat
				//	("Wfc::_calc_least_entropy_pos_darr(): ",
				//	"potential at position ", pos, " has zero tiles!"));
				return std::nullopt;
			}
			//else if (ct.tile_darr.size() == 1)
			else if (tiles.size() == 1) {
				//printout("tile_found ");
				continue;
			}
			std::vector<double> temp_weight_darr, temp_log_weight_darr;

			double temp_entropy = 0.0;
			//for (const auto& tile: tiles)
			//for (const auto& weight: ct.weight_darr)
			for (const auto& tile: tiles) {
				//const double& tile_weight = weight_umap().at(tile.first);
				const double& weight = weight_umap().at(tile);
				temp_entropy -= weight * std::log(weight);
			}
			//temp_entropy = -temp_entropy;
			//printout(temp_entropy, " ");
			if (least_entropy_pos_darr.size() == 0) {
				entropy = temp_entropy;
				least_entropy_pos_darr.push_back(pos);
			} else { // if (least_entropy_pos_darr.size() > 0)
				if (temp_entropy < *entropy) {
					entropy = temp_entropy;
					least_entropy_pos_darr.clear();
				}
				least_entropy_pos_darr.push_back(pos);
			}
		}
		//printout("\n");
	}
	if (entropy) {
		// Break ties at random
		//printout("Wfc::_calc_least_entropy_pos_darr(): ");
		//for (const auto& entropy: least_entropy_pos_darr) {
		//	printout(entropy, " ");
		//}
		//printout("\n");
		//return least_entropy_pos_darr.at
		//	(rng_run<size_t>(_rng,
		//		size_t(0), least_entropy_pos_darr.size() - 1));
		return least_entropy_pos_darr;
	} else {
		//printout("Wfc::_calc_least_entropy_pos_darr(): ",
		//	"Returning `std::nullopt`.\n");
		return std::nullopt;
	}
	//std::sort(entropy_darr.begin(), entropy_darr.end());
}
void Wfc::_propagate(
	Potential& potential,
	const Vec2<size_t>& start_pos
) {
	std::queue<Vec2<size_t>> needs_update;
	needs_update.push(start_pos);
	while (needs_update.size() > 0) {
		//printout("needs_update.size(): ", needs_update.size(), "\n");
		//for (const auto& pos: needs_update) {
		//	printout(pos, " ");
		//}
		//printout("\n");

		if (const Vec2<size_t>& pos=needs_update.front(); true) {
			needs_update.pop();
			const std::vector<Neighbor>& neighbors = _neighbors(pos);
			for (const auto& neighbor: neighbors) {
				_add_constraint(potential, pos, neighbor,
					needs_update);
			}
		}
	}
}
void Wfc::_add_constraint(
	Potential& potential,
	const Vec2<size_t>& pos,

	// `neighbor` stores the incoming `Dir`, and the outgoing `Dir` can be
	// computed with `reverse()`
	const Neighbor& neighbor,
	std::queue<Vec2<size_t>>& needs_update
) {
	PotElem& nb_pot_elem
		= potential.at(neighbor.pos.y).at(neighbor.pos.x);
	const PotElem& tiles = potential.at(pos.y).at(pos.x);

	PotElem to_erase_uset;

	for (const size_t& other_tile: nb_pot_elem) {
		bool found = false;
		//for (const size_t& curr_tile: ct.tile_darr)
		for (const size_t& curr_tile: tiles) {
			if (
				//rule_uset().contains(Rule
				//	{.t0=curr_tile, .t1=other_tile, .d=neighbor.d})
				r2w_umap().contains(Rule
					{.t0=curr_tile, .t1=other_tile, .d=neighbor.d})
				//rule_uset().contains(Rule
				//	{.t0=other_tile,
				//	.t1=curr_tile,
				//	.d=reverse(neighbor.d)})
			) {
				found = true;
				break;
			}
		}
		if (!found) {
			to_erase_uset.insert(other_tile);
			needs_update.push(neighbor.pos);
		}
	}
	for (const size_t& to_erase_tile: to_erase_uset) {
		nb_pot_elem.erase(to_erase_tile);
	}

	if (nb_pot_elem.size() == 0) {
		//_dbg_print();
		//sleep(1);
		throw std::runtime_error(sconcat
			("Wfc::_add_constraint(): ",
			"potential at position ", neighbor.pos, " has zero tiles!"));
	}

	//return changed;
}
auto Wfc::_neighbors(
	const Vec2<size_t>& pos
) const -> std::vector<Neighbor> {
	std::vector<Neighbor> ret;

	if (pos.x > 0) {
		ret.push_back({.d=Dir::Left, .pos{pos.x - 1, pos.y}});
	}
	if (pos.y > 0) {
		ret.push_back({.d=Dir::Top, .pos{pos.x, pos.y - 1}});
	}
	if (pos.x < size_2d().x - size_t(1)) {
		ret.push_back({.d=Dir::Right, .pos{pos.x + 1, pos.y}});
	}
	if (pos.y < size_2d().y - size_t(1)) {
		ret.push_back({.d=Dir::Bottom, .pos{pos.x, pos.y + 1}});
	}

	return ret;
}
void Wfc::_dbg_print() const {
	Vec2<size_t> pos;
	for (pos.y=0; pos.y<size_2d().y; ++pos.y) {
		//printout(pos.y, ": ");
		for (pos.x=0; pos.x<size_2d().x; ++pos.x) {
			const auto& pot_elem
				= _baktk_stk.top().potential.at(pos.y).at(pos.x);
			if (pot_elem.size() == 1) {
				printout(static_cast<char>
					(mt_darr().at(*pot_elem.begin()).tl_corner()));
			} else {
				printout(pot_elem.size());
			}
		}
		printout("\n");
	}
}
//--------
} // namespace wfc

