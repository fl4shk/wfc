#include "wfc_class.hpp"
#include <cmath>
//#include <algorithm>
//#include <unistd.h>

namespace wfc {
//--------
std::vector<Neighbor> calc_neighbors(
	const Vec2<size_t>& full_size_2d, const Vec2<size_t>& pos
) {
	std::vector<Neighbor> ret;

	if (pos.x > 0) {
		ret.push_back({.d=Dir::Left, .pos{pos.x - 1, pos.y}});
	}
	if (pos.y > 0) {
		ret.push_back({.d=Dir::Top, .pos{pos.x, pos.y - 1}});
	}
	if (pos.x < full_size_2d.x - size_t(1)) {
		ret.push_back({.d=Dir::Right, .pos{pos.x + 1, pos.y}});
	}
	if (pos.y < full_size_2d.y - size_t(1)) {
		ret.push_back({.d=Dir::Bottom, .pos{pos.x, pos.y + 1}});
	}

	return ret;
}
//--------
void PotElem::_set(
	Potential& potential,
	std::vector<Vec2<size_t>>& modded_chunk_pos_darr,
	const Vec2<size_t>& chunk_size_2d,
	const Vec2<size_t>& pos, size_t ti, bool val
) {
	PotElem& self = potential.at(pos.y).at(pos.x);
	//const bool did_contain = self.contains(ti);
	//if (pos == Vec2<size_t>(0, 0)) {
	//	printout("PotElem::_set(): ", ti, " ", val, "\n");
	//}
	self.domain.at(ti) = val;

	//modded_chunk_pos_darr.insert(chunk_pos);
	const Vec2<size_t> chunk_pos
		(pos.x / chunk_size_2d.x, pos.y / chunk_size_2d.y);
	bool found = false;
	// It's assumed that `modded_chunk_pos_darr` will be small
	for (const auto& item: modded_chunk_pos_darr) {
		if (item == chunk_pos) {
			found = true;
			break;
		}
	}
	if (!found) {
		modded_chunk_pos_darr.push_back(chunk_pos);
	}

	//const std::vector<Neighbor>& neighbors(calc_neighbors
	//	(Vec2<size_t>(potential.front().size(), potential.size()), pos));
	//for (const Neighbor& nb: neighbors) {
	//	PotElem& nb_pe = potential.at(nb.pos.y).at(nb.pos.x);
	//	auto
	//		& self_support
	//			= self.support_da2d.at(size_t(nb.d)).at(ti),
	//		& nb_support
	//			= nb_pe.support_da2d.at(size_t(reverse(nb.d))).at(ti);
	//	if (val && !did_contain) {
	//		++self_support;
	//		++nb_support;
	//	} else if (!val && did_contain) {
	//		--self_support;
	//		--nb_support;
	//		//printout("!val && did_contain: ",
	//		//	ti, "; ",
	//		//	pos, " ", nb.pos, "; ",
	//		//	self_support, " ", nb_support,
	//		//	"\n");
	//		//if (self_support == 0) {
	//		//	//printout("self_support == 0\n");
	//		//	self.domain.at(ti) = false;
	//		//} //else {
	//		////	self.domain.at(ti) = true;
	//		////}
	//		//if (nb_support == 0) {
	//		//	//printout("nb_support == 0\n");
	//		//	nb_pe.domain.at(ti) = false;
	//		//} //else {
	//		////	nb_pe.domain.at(ti) = true;
	//		////}
	//	}
	//	if (self_support == 0) {
	//		//printout("self_support == 0\n");
	//		self.domain.at(ti) = false;
	//	} else {
	//		self.domain.at(ti) = true;
	//	}
	//	if (nb_support == 0) {
	//		//printout("nb_support == 0\n");
	//		nb_pe.domain.at(ti) = false;
	//	} else {
	//		nb_pe.domain.at(ti) = true;
	//	}

	//	//if (val && !did_contain) {
	//	//	++self_support;
	//	//	//++nb_support;
	//	//} else if (!val && did_contain) {
	//	//	--self_support;
	//	//	//--nb_support;
	//	//}
	//	//if (did_contain && self_support == 0) {
	//	//	self.domain.at(ti) = false;
	//	//} else if (!did_contain && self_support > 0) {
	//	//	self.domain.at(ti) = true;
	//	//}
	//}
}
//--------
Wfc::Wfc() {}
Wfc::Wfc(
	const Vec2<size_t>& s_chunk_size_2d,
	const Vec2<size_t>& s_num_chunks_2d,
	size_t s_mt_dim,
	//const std::vector<std::vector<size_t>>& input_tiles,
	bool s_opt_debug_print,
	bool s_opt_backtrack,
	bool s_opt_overlap,
	bool s_opt_rotate, bool s_opt_reflect,
	u64 s_rng_seed
) 
	: _chunk_size_2d(s_chunk_size_2d),
	_num_chunks_2d(s_num_chunks_2d),
	_full_size_2d
		(s_chunk_size_2d.x * s_num_chunks_2d.x,
		s_chunk_size_2d.y * s_num_chunks_2d.y),

	_mt_dim(s_opt_overlap ? s_mt_dim : 1),
	//_potential(s_size_2d.y,
	//	std::vector<TileUset>(s_size_2d.x, TileUset())),
	_opt_debug_print(s_opt_debug_print),
	_opt_backtrack(s_opt_backtrack),
	_opt_overlap(s_opt_overlap),
	_opt_rotate(s_opt_rotate),
	_opt_reflect(s_opt_reflect),
	_rng(s_rng_seed) {
	//--------
	//#ifdef DEBUG
	//printout("wfc::Wfc::Wfc(): s_rng_seed: ", s_rng_seed, "\n");
	//#endif		// DEBUG
	_result = Potential(full_size_2d().y,
		std::vector<PotElem>(full_size_2d().x, PotElem()));
	//_orig_state = Potential(full_size_2d().y,
	//	std::vector<PotElem>(full_size_2d().x, PotElem()));

	if (mt_dim() > full_size_2d().x) {
		throw std::invalid_argument(sconcat
			("Wfc::Wfc(): Error: ",
			"_mt_dim (", mt_dim(), ") is greater than ",
			"_full_size_2d.x (", full_size_2d().x, ")."));
	}
	if (mt_dim() > full_size_2d().y) {
		throw std::invalid_argument(sconcat
			("Wfc::Wfc(): Error: ",
			"_mt_dim (", mt_dim(), ") is greater than ",
			"_full_size_2d.y (", full_size_2d().y, ")."));
	}
	//learn(input_tiles);
	//gen();
	//--------
}
Wfc::~Wfc() {}

void Wfc::learn(const std::vector<std::vector<size_t>>& input_tiles) {
	//--------
	// It's assumed that `input_tiles` has a small overall size
	if (input_tiles.size() == 0) {
		throw std::invalid_argument(sconcat
			("`input_tiles.size()` is zero"));
	}
	//for (const auto& row: input_tiles) 
	for (size_t j=0; j<input_tiles.size(); ++j) {
		const auto& row = input_tiles.at(j);
		if (row.size() == 0) {
			throw std::invalid_argument(sconcat
				("input_tiles.at(", j, ").size() is zero"));
		} else if (j > 0 && row.size() != input_tiles.at(j - 1).size()) {
			throw std::invalid_argument(sconcat
				("input_tiles.at(", j, ").size() (", row.size(), ") ",
				"is not equal to ",
				"input_tiles.at(", j - 1, ").size() ",
				"(", input_tiles.at(j - 1).size(), ")"));
		}
	}
	//_tprops_umap.clear();
	//Potential potential;
	_result.clear();
	_orig_state.clear();
	//_potential.clear();
	//_rules_umap.clear();
	//_rule_uset.clear();
	_mt_darr.clear();
	_r2w_umap.clear();
	_weight_darr.clear();

	//PotElem _default_pe;
	//if (std::holds_alternative<std::monostate>(_default_pe)) {
	//	_default_pe = PotElem();
	//}
	_default_pe = PotElem();

	// Insert weights
	//for (const auto& item: mt_darr()) 
	//for (size_t i=0; i<mt_darr().size(); ++i) {
	//	insert_weight(i);
	//}
	//auto insert_weight = [&](const size_t& item) -> void {
	//	// Insert weights, so that tiles that appear more often in
	//	// `input_tiles` have a higher weight
	//	//const size_t& item = row.at(i);
	//	_default_pe.insert(item);
	//	if (temp_weight_umap.contains(item)) {
	//		//_weight_umap.at(item) += 3.0;
	//		temp_weight_umap.at(item) += 1.0;
	//	} else { // if (!temp_weight_umap.contains(item))
	//		temp_weight_umap.insert(std::pair(item, 1.0));
	//	}
	//};

	// Insert metatiles
	std::unordered_map<Metatile, size_t> mt_id_umap;
	if (std::unordered_map<Metatile, double> mt_umap; true) {
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
				const auto& mt = traverse(j, i);
				//mt_umap.insert(traverse(j, i));
				if (!mt_umap.contains(mt)) {
					mt_umap.insert(std::pair(mt, 1.0));
				} else {
					mt_umap.at(mt) += 1.0;
				}
			}
		}

		if (opt_overlap()) {
			std::unordered_map<Metatile, double> ext_mt_umap;
			for (const auto& item: mt_umap) {
				auto do_mt_insert = [&](
					const Metatile& to_insert
				) -> void {
					//if (!ext_mt_umap.contains(mt)) {
					//	ext_mt_umap.insert(std::pair(to_insert, 1.0));
					//} else {
					//	ext_mt_umap.at(to_insert) += 1.0;
					//}
					// Use weights equal to that of the the non-rotated,
					// non-reflected `Metatile`'s weights
					ext_mt_umap.insert(std::pair(to_insert, item.second));
				};
				if (opt_reflect()) {
					do_mt_insert(Metatile(item.first).reflect_x());
					do_mt_insert(Metatile(item.first).reflect_y());
					do_mt_insert
						(Metatile(item.first).reflect_x().reflect_y());
				}
				if (opt_rotate()) {
					auto rot90 = Metatile(item.first).rotate_p90();
					do_mt_insert(rot90);
					if (opt_reflect()) {
						do_mt_insert(Metatile(rot90).reflect_x());
						do_mt_insert(Metatile(rot90).reflect_y());
						do_mt_insert
							(Metatile(rot90).reflect_x().reflect_y());
					}

					auto rot180 = Metatile(rot90).rotate_p90();
					do_mt_insert(rot180);
					if (opt_reflect()) {
						do_mt_insert(Metatile(rot180).reflect_x());
						do_mt_insert(Metatile(rot180).reflect_y());
						do_mt_insert
							(Metatile(rot180).reflect_x().reflect_y());
					}

					auto rot270 = Metatile(rot180).rotate_p90();
					do_mt_insert(rot270);
					if (opt_reflect()) {
						do_mt_insert(Metatile(rot270).reflect_x());
						do_mt_insert(Metatile(rot270).reflect_y());
						do_mt_insert
							(Metatile(rot270).reflect_x().reflect_y());
					}
				}
			}
			mt_umap.merge(std::move(ext_mt_umap));
		}

		//if (size_t i=0; true) {
			for (const auto& item: mt_umap) {
				mt_id_umap.insert(std::pair
					(item.first, mt_darr().size()));
				//_default_pe.insert(i);
				//_default_pe.insert(i);
				//_default_pe.data.push_back(true);
				//_default_pe.push_back(i);
				//_default_pe.push_back(mt_darr().size());
				//_default_pe.append();
				//_default_pe.data.push_back(1);
				//_default_pe.data.push_back("asdf");
				//_default_pe.insert_maybe(i);
				_mt_darr.push_back(item.first);
				_weight_darr.push_back(item.second);
				//++i;
			}
		//}
		_default_pe = PotElem(mt_darr().size());
	}

	//printout("Inserting weights\n");
	//for (i32 j=0; j<i32(input_tiles.size()); ++j) {
	//	const auto& row = input_tiles.at(j);
	//	//printout(row, "\n");
	//	for (i32 i=0; i<i32(row.size()); ++i) {
	//		insert_weight(row.at(i);
	//	}
	//}
	//printout("learn(): `_default_pe`: ");
	//for (size_t i=0; i<_default_pe.size(); ++i) {
	//	if (_default_pe.at(i)) {
	//		printout(i);
	//	} else {
	//		printout("?");
	//	}
	//}
	//printout("\n");
	_result = Potential(full_size_2d().y,
		std::vector<PotElem>(full_size_2d().x, _default_pe));
	_orig_state = Potential(full_size_2d().y,
		std::vector<PotElem>(full_size_2d().x, _default_pe));
	//_result = _orig_state;
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
	if (opt_overlap()) {
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
	} else { // if (!opt_overlap())
		for (i32 j=0; j<i32(input_tiles.size()); ++j) {
			const auto& row = input_tiles.at(j);
			for (i32 i=0; i<i32(row.size()); ++i) {
				const auto& item = row.at(i);
				//auto get_metatile = [&]
				//auto insert_rule = [&](const Rule& rule) -> void {
				//	//printout("in insert_rule():\n");
				//	//if (!rule_uset().contains(rule))
				//	if (!r2w_umap().contains(rule)) {
				//		//_rule_uset.insert(rule);
				//		_r2w_umap.insert(std::pair(rule, 1.0));
				//	} else { // if (r2w_umap().contains(rule))
				//		_r2w_umap.at(rule) += 1.0;
				//	}
				//};
				// left
				if (i > 0) {
					//printout("i > 0: ");
					//const Rule rule{item, row.at(i - 1), Dir::Left};
					Metatile mt0(1), mt1(1);
					mt0.front() = item;
					mt1.front() = row.at(i - 1);
					const Rule rule
						{.t0=mt_id_umap.at(mt0),
						.t1=mt_id_umap.at(mt1),
						.d=Dir::Left};
					//const Rule rule{row.at(i - 1), item, Dir::Left};
					insert_rule(rule);
					insert_rule(rule.reverse());
				}
				// top
				if (j > 0) {
					//printout("j > 0: ");
					//const Rule rule{item, input_tiles.at(j - 1).at(i), 
					//	Dir::Top};

					Metatile mt0(1), mt1(1);
					mt0.front() = item;
					mt1.front() = input_tiles.at(j - 1).at(i);
					const Rule rule
						{.t0=mt_id_umap.at(mt0),
						.t1=mt_id_umap.at(mt1),
						.d=Dir::Top};

					insert_rule(rule);
					insert_rule(rule.reverse());
				}
				// right
				if (i + 1 != i32(row.size())) {
					//printout("i + 1 != i32(row.size()): ");
					//const Rule rule{item, row.at(i + 1), Dir::Right};

					Metatile mt0(1), mt1(1);
					mt0.front() = item;
					mt1.front() = row.at(i + 1);
					const Rule rule
						{.t0=mt_id_umap.at(mt0),
						.t1=mt_id_umap.at(mt1),
						.d=Dir::Right};
					insert_rule(rule);
					insert_rule(rule.reverse());
				}
				// bottom
				if (j + 1 != i32(input_tiles.size())) {
					//printout("j + 1 != i32(input_tiles.size()): ");
					//const Rule rule{item, input_tiles.at(j + 1).at(i),
					//	Dir::Bottom};

					Metatile mt0(1), mt1(1);
					mt0.front() = item;
					mt1.front() = input_tiles.at(j + 1).at(i);
					const Rule rule
						{.t0=mt_id_umap.at(mt0),
						.t1=mt_id_umap.at(mt1),
						.d=Dir::Bottom};
					insert_rule(rule);
					insert_rule(rule.reverse());
				}
			}
		}
	}
	//--------
}
void Wfc::copy_knowledge(
	const std::optional<MtDarr>& n_mt_darr,
	const std::optional<R2wUmap>& n_r2w_umap,
	const std::optional<WeightDarr>& n_weight_darr
) {
	if (n_mt_darr) {
		_mt_darr = *n_mt_darr;
	}
	if (n_r2w_umap) {
		_r2w_umap = *n_r2w_umap;
	}
	if (n_weight_darr) {
		_weight_darr = *n_weight_darr;
	}
}
void Wfc::copy_knowledge(const Wfc& to_copy) {
	if (mt_dim() != to_copy.mt_dim()) {
		throw std::invalid_argument(sconcat
			("wfc::Wfc::copy_knowledge(): ",
			"`mt_dim() (", mt_dim(), ") "
			"!= to_copy.mt_dim() (", to_copy.mt_dim(), ")`"));
	}
	if (opt_overlap() != to_copy.opt_overlap()) {
		throw std::invalid_argument(sconcat
			("wfc::Wfc::copy_knowledge(): ",
			"`opt_overlap() (", opt_overlap(), ") "
			"!= to_copy.opt_overlap() (", to_copy.opt_overlap(), ")`"));
	}
	if (opt_rotate() != to_copy.opt_rotate()) {
		throw std::invalid_argument(sconcat
			("wfc::Wfc::copy_knowledge(): ",
			"`opt_rotate() (", opt_rotate(), ") "
			"!= to_copy.opt_rotate() (", to_copy.opt_rotate(), ")`"));
	}
	if (opt_reflect() != to_copy.opt_reflect()) {
		throw std::invalid_argument(sconcat
			("wfc::Wfc::copy_knowledge(): ",
			"`opt_reflect() (", opt_reflect(), ") "
			"!= to_copy.opt_reflect() (", to_copy.opt_reflect(), ")`"));
	}
	copy_knowledge
		(to_copy.mt_darr(), to_copy.r2w_umap(), to_copy.weight_darr());
}
void Wfc::gen() {
	//while (_gen_iteration()) {
	//}
	//if (BaktkStkItem to_push{.potential=_result}; true) {
	//	auto temp_le_pos_darr
	//		= _calc_least_entropy_pos_darr(to_push.potential);
	//	_baktk_stk.push_back(std::move(to_push));
	//}
	//std::optional<PosDarr> least_entropy_pos_darr = std::nullopt;
	//const PotElem orig_pe = _result.front().front();

	//_baktk_stk.push_back(BaktkStkItem
	//	{.potential=_result});
	_result = _orig_state;
	//_baktk_stk.push_back(BaktkStkItem
	//	{.potential=_result});
	//_baktk_stk.back().least_entropy_pos_darr
	//	= *_calc_least_entropy_pos_darr(_baktk_stk.back().potential,
	//		{0, 0});
	////_baktk_stk.back().init_guess_umap();
	//_baktk_stk.back().init_guess_darr();

	//bool
	//	did_pop = false;
	Vec2<size_t> chunk_pos;
	for (chunk_pos.y=0; chunk_pos.y<num_chunks_2d().y; ++chunk_pos.y) {
		for (chunk_pos.x=0; chunk_pos.x<num_chunks_2d().x; ++chunk_pos.x) {
			if (opt_backtrack()) {
				_baktk_stk.clear();
			}
			if (
				opt_backtrack()
				|| chunk_pos == Vec2<size_t>(0, 0)
			) {
				_baktk_stk.push_back(BaktkStkItem
					{.potential=_result});
				_baktk_stk.back().least_entropy_pos_darr
					= *_calc_least_entropy_pos_darr
						(_baktk_stk.back().potential, chunk_pos);
				//_baktk_stk.back().init_guess_umap();
				_baktk_stk.back().init_guess_darr();
			}
			//std::vector<Vec2<size_t>> modded_chunk_pos_darr;
			for (;;) {
				//printout("_baktk_stk.size(): ", _baktk_stk.size(), "; ");
				BaktkStkItem& stk_top = _baktk_stk.back();
				//stk_top.modded_chunk_pos_darr.clear();

				Potential& potential = stk_top.potential;
				//printout("_stk_top.potential: ", potential.size());
				//if (potential.size() > 0) {
				//	printout(" ", potential.front().size());
				//}
				//printout("\n");

				//auto& guess_umap = stk_top.guess_umap;
				auto& guess_darr = stk_top.guess_darr;
				size_t& guess_index = stk_top.guess_index;
				Vec2<size_t>& guess_pos = stk_top.guess_pos;
				size_t& guess_ti = stk_top.guess_ti;

				//if (did_pop) {
				//	did_pop = false;
				//}

				//guess_index = rng_run<size_t>(_rng,
				//	size_t(0), guess_umap.size() - 1);
				guess_index = rng_run<size_t>(_rng,
					size_t(0), guess_darr.size() - 1);
				stk_top.init_guess_pos();
				//--------
				BaktkStkItem to_push;

				if (opt_backtrack()) {
					to_push.potential = potential;
					to_push.modded_chunk_pos_darr
						= stk_top.modded_chunk_pos_darr;
					//to_push.modded_chunk_pos_darr
				} else {
					to_push.potential = std::move(potential);
					to_push.modded_chunk_pos_darr
						= std::move(stk_top.modded_chunk_pos_darr);
				}

				auto& to_collapse
					= to_push.potential.at(guess_pos.y).at(guess_pos.x);
				const CollapseTemps& ct
					//= _calc_collapse_temps(guess_umap, guess_pos);
					= _calc_collapse_temps(to_push.potential, guess_pos);
					//= _calc_collapse_temps(potential, guess_pos);
				Ddist ddist(ct.modded_weight_darr.begin(),
					ct.modded_weight_darr.end());
				//to_collapse.clear();
				//for (size_t i=0; i<to_collapse.data.size(); ++i)
				for (size_t i=0; i<default_pe().domain.size(); ++i) {
					to_collapse.disable(to_push.potential,
						to_push.modded_chunk_pos_darr, chunk_size_2d(),
						guess_pos, i);
				}

				const auto& rng_val = ddist(_rng);
				guess_ti = ct.tile_darr.at(rng_val);
				//to_collapse.insert(guess_ti);
				to_collapse.enable(to_push.potential,
					to_push.modded_chunk_pos_darr, chunk_size_2d(),
					guess_pos, guess_ti);

				//to_collapse.at(guess_ti) = 1;

				bool failed = false;
				try {
					_propagate(to_push.potential,
						to_push.modded_chunk_pos_darr,
						guess_pos);
				} catch (const std::exception& e) {
					failed = true;
					// We failed to `_propagate()`.
					//#ifdef DEBUG
					//printout("failed `_propagate()`: ",
					//	_baktk_stk.size(),
					//	"\n");
					//_dbg_print(_baktk_stk.back().potential);
					//_dbg_print(to_push.potential);
					//#endif		// DEBUG

					//_baktk_stk.back().modded_chunk_pos_darr

					//if (opt_backtrack()) {
					//	//need_pop = true;
					//	_baktk_stk.back().erase_guess
					//		(to_push.modded_chunk_pos_darr,
					//		chunk_size_2d());
					//	//if (_baktk_stk.back().guess_umap.size() == 0)
					//	if (_baktk_stk.back().guess_darr.size() == 0) {
					//		//#ifdef DEBUG
					//		printout("failed `_propagate()`: ",
					//			"doing `_baktk_stk.pop_back()`\n");
					//		//#endif		// DEBUG
					//		//did_pop = true;
					//		_baktk_stk.pop_back();
					//	}

					//	//continue;
					//} else { // if (!opt_backtrack())
					//	//#ifdef DEBUG
					//	//printout("restarting\n");
					//	//#endif		// DEBUG
					//	//return;
					//}
				}

				//if (opt_backtrack()) {
				//	//_baktk_stk.back().potential = to_push.potential;
				//} else { // if (!opt_backtrack())
				//}

				if (opt_backtrack()) {
					if (!failed) {
						//printout("opt_backtrack() && !failed: ");
						auto temp_least_entropy_pos_darr
							= _calc_least_entropy_pos_darr
								(to_push.potential, chunk_pos);
						if (!temp_least_entropy_pos_darr) {
							//_result = std::move(to_push.potential);
							//_result = to_push.potential;
							//copy_chunk(_result, to_push.potential, chunk_pos);
							_copy_modded_chunks(_result,
								to_push.potential,
								to_push.modded_chunk_pos_darr);
							break;
						}
						to_push.least_entropy_pos_darr
							= *temp_least_entropy_pos_darr;
						//to_push.init_guess_umap();
						to_push.init_guess_darr();

						_baktk_stk.push_back(std::move(to_push));
					} else { // if (failed)
						//need_pop = true;
						_baktk_stk.back().erase_guess
							(to_push.modded_chunk_pos_darr,
							chunk_size_2d());
						//if (_baktk_stk.back().guess_umap.size() == 0)
						if (_baktk_stk.back().guess_darr.size() == 0) {
							//#ifdef DEBUG
							//printout("failed `_propagate()`: ",
							//	"doing `_baktk_stk.pop_back()`\n");
							//#endif		// DEBUG
							//did_pop = true;
							_baktk_stk.pop_back();
						}
					}
				} else { // if (!opt_backtrack())
					if (!failed) {
						_baktk_stk.back().potential
							= std::move(to_push.potential);
							//= to_push.potential;
						_baktk_stk.back().modded_chunk_pos_darr
							= std::move(to_push.modded_chunk_pos_darr);
							//= to_push.modded_chunk_pos_darr;

						auto temp_least_entropy_pos_darr
							= _calc_least_entropy_pos_darr
								(_baktk_stk.back().potential, chunk_pos);
						if (!temp_least_entropy_pos_darr) {
							//_result
							//	= std::move(_baktk_stk.back().potential);
							//copy_chunk(_result,
							//	_baktk_stk.back().potential,
							//	chunk_pos);
							_copy_modded_chunks(_result,
								_baktk_stk.back().potential,
								_baktk_stk.back().modded_chunk_pos_darr);
							//_result = _baktk_stk.back().potential;
							break;
						}
						_baktk_stk.back().least_entropy_pos_darr
							= *temp_least_entropy_pos_darr;
						//to_push.init_guess_umap();
						_baktk_stk.back().init_guess_darr();
						//_baktk_stk.back() = std::move(to_push);
					} else { // if (failed)
						//const auto& temp_pe
						//	= to_push.potential.front().front();
						//printout(temp_pe.data.size(), " ",
						//	temp_pe.num_active(), "\n");
						//_baktk_stk.push_back(std::move(to_push));
						//printout("_dbg_print(): \n");
						//_dbg_print(_baktk_stk.back().potential);
						//_baktk_stk.pop_back();

						//_baktk_stk.back().potential
						//	= Potential(full_size_2d().y,
						//		std::vector<PotElem>
						//			(full_size_2d().x, _default_pe));
						_baktk_stk.back().potential
							//= _orig_state;
							= _result;
						//_copy_modded_chunks
						//	(_baktk_stk.back().potential, _result,
						//	_baktk_stk.back().modded_chunk_pos_darr);
						_baktk_stk.back().least_entropy_pos_darr
							= *_calc_least_entropy_pos_darr
								(_baktk_stk.back().potential, chunk_pos);
						_baktk_stk.back().init_guess_darr();
					}
				}

				//if (opt_backtrack()) {
				//} else { // if (restarting)
				//}
				//--------
			}
			if (opt_debug_print()) {
				_dbg_print(_result);
				printout("\n");
			}
		}
	}
	//if (opt_debug_print()) {
	//	printout("result:\n");
	//}

	//_post_process();
}
void Wfc::set_orig_state(const PotentialUmap& to_inject) {
	//_result = Potential(full_size_2d().y,
	//	std::vector<PotElem>(full_size_2d().x, _default_pe));
	_orig_state = Potential(full_size_2d().y,
		std::vector<PotElem>(full_size_2d().x, _default_pe));

	for (const auto& item: to_inject) {
		const auto& pos = item.first;
		if (
			pos.x >= full_size_2d().x
			|| pos.y >= full_size_2d().y
		) {
			throw std::out_of_range(sconcat
				("wfc::Wfc::set_orig_state(): position (", pos, ") ",
				"out of range for full_size_2d() ",
				"(", full_size_2d(), ")."));
		}
		orig_state_at(pos) = item.second;
	}
}

void Wfc::set_orig_state(PotentialUmap&& to_inject) {
	//_result = Potential(full_size_2d().y,
	//	std::vector<PotElem>(full_size_2d().x, _default_pe));
	_orig_state = Potential(full_size_2d().y,
		std::vector<PotElem>(full_size_2d().x, _default_pe));

	for (auto& item: to_inject) {
		const auto& pos = item.first;
		if (
			pos.x >= full_size_2d().x
			|| pos.y >= full_size_2d().y
		) {
			throw std::out_of_range(sconcat
				("wfc::Wfc::set_orig_state(): position (", pos, ") ",
				"out of range for full_size_2d() ",
				"(", full_size_2d(), ")."));
		}
		orig_state_at(pos) = std::move(item.second);
	}
}

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
		//for (const auto& item: pot_elem)
		//for (size_t item=0; item<pot_elem.data.size(); ++item)
		for (size_t ti=0; ti<default_pe().domain.size(); ++ti) {
			if (pot_elem.contains(ti)) {
				//ret.weight_darr.push_back(weight_umap().at(ti.first));
				//ret.tile_darr.push_back(ti.first);
				//ret.tid_umap.insert(std::pair(ti.first, i));
				//ret.weight_darr.push_back(weight_umap().at(ti));
				ret.modded_weight_darr.push_back(_calc_modded_weight
					(potential, pos, ti));
				//ret.modded_weight_darr.push_back(_calc_modded_weight
				//	(pot_umap, pos, ti));
				ret.tile_darr.push_back(ti);
				ret.tid_umap.insert(std::pair(ti, i));
				++i;
			}
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
					//nb_pot_elem.size() == 1
					nb_pot_elem.num_active() == 1
					//&& r2w_pair.first.t1 == *nb_pot_elem.begin()
					&& r2w_pair.first.t1 == *nb_pot_elem.first_set()
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
					//nb_pot_elem.size() == 1
					nb_pot_elem.num_active() == 1
					//&& r2w_pair.first.t0 == *nb_pot_elem.begin()
					&& r2w_pair.first.t0 == *nb_pot_elem.first_set()
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
		ret = weight_darr().at(item);
	}
	return ret;
}
auto Wfc::_calc_least_entropy_pos_darr(
	const Potential& potential, const Vec2<size_t>& chunk_pos
) -> std::optional<PosDarr> {
	std::optional<double> entropy = std::nullopt;
	PosDarr least_entropy_pos_darr;

	Vec2<size_t> pos;
	//printout("Wfc::_calc_least_entropy_pos_darr(): computing...\n");
	for (
		pos.y=chunk_pos.y * chunk_size_2d().y;
		pos.y<(chunk_pos.y + 1) * chunk_size_2d().y;
		++pos.y
	) {
		for (
			pos.x=chunk_pos.x * chunk_size_2d().x;
			pos.x<(chunk_pos.x + 1) * chunk_size_2d().x;
			++pos.x
		) {
			const auto& tiles = potential.at(pos.y).at(pos.x);
			//const CollapseTemps ct = _calc_collapse_temps(pos);
			//if (ct.tile_darr.size() == 0)
			//if (tiles.size() == 0) 
			if (tiles.num_active() == 0) {
				// We've failed the generation at this point.
				//_dbg_print();
				//sleep(1);
				//throw std::runtime_error(sconcat
				//	("Wfc::_calc_least_entropy_pos_darr(): ",
				//	"potential at position ", pos, " has zero tiles!"));
				return std::nullopt;
			}
			//else if (ct.tile_darr.size() == 1)
			//else if (tiles.size() == 1)
			else if (tiles.num_active() == 1) {
				//printout("tile_found ");
				continue;
			}
			std::vector<double> temp_weight_darr, temp_log_weight_darr;

			double temp_entropy = 0.0;
			//for (const auto& tile: tiles)
			//for (const auto& weight: ct.weight_darr)
			//for (const auto& tile: tiles) 
			//for (size_t ti=0; ti<tiles.data.size(); ++ti)
			for (size_t ti=0; ti<default_pe().domain.size(); ++ti) {
				if (tiles.contains(ti)) {
					//const double& tile_weight = weight_umap()
					//	.at(ti.first);
					const double& weight = weight_darr().at(ti);
					temp_entropy -= weight * std::log(weight);
				}
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
	std::vector<Vec2<size_t>>& modded_chunk_pos_darr,
	const Vec2<size_t>& start_pos
) {
	std::queue<Vec2<size_t>> needs_update;
	needs_update.push(start_pos);
	while (needs_update.size() > 0) {
		if (
			const Vec2<size_t>& pos=needs_update.front();
			//const Vec2<size_t>& pos=needs_update.top();
			true
		) {
			needs_update.pop();
			const std::vector<Neighbor>& neighbors = _neighbors(pos);
			for (const auto& neighbor: neighbors) {
				_add_constraint(potential,
					modded_chunk_pos_darr,
					pos, neighbor, needs_update);
			}
		}
	}

	//printout("wfc::Wfc::_propagate(): here are my results:\n");
	//_dbg_print();
	//printout("\n");
}
void Wfc::_add_constraint(
	Potential& potential,
	std::vector<Vec2<size_t>>& modded_chunk_pos_darr,
	const Vec2<size_t>& pos,

	// `neighbor` stores the incoming `Dir`, and the outgoing `Dir` can be
	// computed with `reverse()`
	const Neighbor& neighbor,
	std::queue<Vec2<size_t>>& needs_update
	//std::stack<Vec2<size_t>>& needs_update
) {
	const PotElem& tiles = potential.at(pos.y).at(pos.x);
	PotElem& nb_pot_elem
		= potential.at(neighbor.pos.y).at(neighbor.pos.x);
	//printout("debug: _add_constraint(): tiles: ", tiles, "\n");

	//std::unordered_set<size_t>
	//	//tiles,
	//	//other_tiles,
	//	to_erase_uset;
	std::vector<size_t> to_erase_darr;
	//for (size_t i=0; i<raw_tiles.size(); ++i) {
	//	if (raw_tiles.contains(i)) {
	//		tiles.insert(i);
	//	}
	//}
	//for (size_t i=0; i<nb_pot_elem.size(); ++i) {
	//	if (nb_pot_elem.contains(i)) {
	//		other_tiles.insert(i);
	//	}
	//}

	//for (const size_t& other_tile: nb_pot_elem) 
	//for (const size_t& other_tile: other_tiles) 
	//for (
	//	size_t other_tile=0;
	//	other_tile<nb_pot_elem.data.size();
	//	++other_tile
	//)
	for (
		size_t other_tile=0;
		other_tile<default_pe().domain.size();
		++other_tile
	) {
		//if (nb_pot_elem.at(other_tile))
		if (nb_pot_elem.contains(other_tile)) {
			//printout("testificate: ", other_tile, "\n");
			bool found = false;
			//for (const size_t& curr_tile: ct.tile_darr)
			//for (const size_t& curr_tile: tiles) 
			//for (
			//	size_t curr_tile=0;
			//	curr_tile<tiles.data.size();
			//	++curr_tile
			//)
			for (
				size_t curr_tile=0;
				curr_tile<default_pe().domain.size();
				++curr_tile
			) {
				if (
					//nb_pot_elem.contains(other_tile)
					tiles.contains(curr_tile)
					//tiles.at(curr_tile)
					//&& rule_uset().contains(Rule
					//	{.t0=curr_tile, .t1=other_tile, .d=neighbor.d})
					//&& 
					&& r2w_umap().contains(Rule
						{.t0=curr_tile, .t1=other_tile, .d=neighbor.d})
					//&& rule_uset().contains(Rule
					//	{.t0=other_tile,
					//	.t1=curr_tile,
					//	.d=reverse(neighbor.d)})
				) {
					found = true;
					break;
				}
			}
			if (!found) {
				//printout("_add_constraint(): !found: ",
				//	other_tile, "\n");
				//to_erase_uset.insert(other_tile);
				bool found_other_tile = false;
				for (const auto& to_erase: to_erase_darr) {
					if (to_erase == other_tile) {
						found_other_tile = true;
						break;
					}
				}
				if (!found_other_tile) {
					to_erase_darr.push_back(other_tile);
				}
				needs_update.push(neighbor.pos);
			}
		}
	}
	//for (const size_t& to_erase_tile: to_erase_uset)
	for (const size_t& to_erase_tile: to_erase_darr)
	{
		//printout("_add_constraint(): Erasing: ",
		//	nb_pot_elem.contains(to_erase_tile), " ",
		//	to_erase_tile, "\n");
		//nb_pot_elem.erase(to_erase_tile);
		nb_pot_elem.disable(potential,
			modded_chunk_pos_darr, chunk_size_2d(),
			neighbor.pos, to_erase_tile);
	}

	//if (nb_pot_elem.size() == 0)
	if (nb_pot_elem.num_active() == 0) {
		//_dbg_print();
		//sleep(1);
		throw std::runtime_error(sconcat
			("Wfc::_add_constraint(): ",
			"potential at position ", neighbor.pos, " has zero tiles!"));
	}

	//return changed;
}
void Wfc::_dbg_print(const Potential& potential) const {
	Vec2<size_t> pos;
	for (pos.y=0; pos.y<full_size_2d().y; ++pos.y) {
		//printout(pos.y, ": ");
		for (pos.x=0; pos.x<full_size_2d().x; ++pos.x) {
			//printout(pos, "\n");
			//printout(bts_item.potential.size());
			//if (bts_item.potential.size() > 0) {
			//	printout(" ", bts_item.potential.front().size());
			//}
			//printout("\n");
			const PotElem& pot_elem
				= potential.at(pos.y).at(pos.x);
			//if (pot_elem.size() == 1)
			if (pot_elem.num_active() == 1) {
				//printout("pot_elem.num_active() == 1\n");
				//printout(static_cast<char>
				//	(mt_darr().at(*pot_elem.begin()).tl_corner()));
				printout(static_cast<char>
					(mt_darr().at(*pot_elem.first_set()).tl_corner()));
			} else {
				//printout("pot_elem.num_active() != 1\n");
				//printout(pot_elem.size());
				if (pot_elem.num_active() <= 9) {
					printout(pot_elem.num_active());
				} else {
					printout("*");
				}
			}
		}
		printout("\n");
	}
}
//--------
} // namespace wfc

