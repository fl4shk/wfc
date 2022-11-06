#include "wfc_class.hpp"
#include <cmath>
//#include <algorithm>
//#include <unistd.h>

namespace wfc {
//--------
Wfc::Wfc() {}
Wfc::Wfc(
	const Vec2<size_t>& s_size_2d, u64 s_rng_seed,
	const std::vector<std::vector<u32>>& input_tiles
) 
	: _size_2d(s_size_2d),
	//_potential(s_size_2d.y,
	//	std::vector<TileUset>(s_size_2d.x, TileUset())),
	_rng(s_rng_seed) {
	//--------
	_learn(input_tiles);
	_gen();
	//--------
}
Wfc::~Wfc() {}

void Wfc::_learn(const std::vector<std::vector<u32>>& input_tiles) {
	//--------
	//_tprops_umap.clear();
	_potential.clear();
	//_rules_umap.clear();
	//_rule_uset.clear();
	_r2w_umap.clear();
	_full_weight_umap.clear();

	PotElem pot_elem;

	// Insert weights first
	//printout("Inserting weights\n");
	for (i32 j=0; j<i32(input_tiles.size()); ++j) {
		const auto& row = input_tiles.at(j);
		//printout(row, "\n");
		for (i32 i=0; i<i32(row.size()); ++i) {
			// Insert weights, so that tiles that appear more often in
			// `input_tiles` have a higher weight
			const u32& item = row.at(i);
			pot_elem.insert(item);
			if (full_weight_umap().contains(item)) {
				_full_weight_umap.at(item) += 1.0;
			} else { // if (!full_weight_umap().contains(item))
				_full_weight_umap.insert(std::pair(item, 1.0));
			}
		}
	}
	_potential = Potential(size_2d().y,
		std::vector<PotElem>(size_2d().x, pot_elem));

	// Insert `Rule`s
	for (i32 j=0; j<i32(input_tiles.size()); ++j) {
		const auto& row = input_tiles.at(j);
		for (i32 i=0; i<i32(row.size()); ++i) {
			const auto& item = row.at(i);
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
			// left
			if (i > 0) {
				//printout("i > 0: ");
				const Rule rule{item, row.at(i - 1), Dir::Left};
				//const Rule rule{row.at(i - 1), item, Dir::Left};
				insert_rule(rule);
				insert_rule(rule.reverse());
			}
			// top
			if (j > 0) {
				//printout("j > 0: ");
				const Rule rule{item, input_tiles.at(j - 1).at(i), 
					Dir::Top};
				insert_rule(rule);
				insert_rule(rule.reverse());
			}
			// right
			if (i + 1 != i32(row.size())) {
				//printout("i + 1 != i32(row.size()): ");
				const Rule rule{item, row.at(i + 1), Dir::Right};
				insert_rule(rule);
				insert_rule(rule.reverse());
			}
			// bottom
			if (j + 1 != i32(input_tiles.size())) {
				//printout("j + 1 != i32(input_tiles.size()): ");
				const Rule rule{item, input_tiles.at(j + 1).at(i),
					Dir::Bottom};
				insert_rule(rule);
				insert_rule(rule.reverse());
			}
		}
	}
	//--------
}
void Wfc::_gen() {
	//do
	//{
	//	printout("_gen(): Here's the current state:\n");
	//	_dbg_print();
	//	//printout("\n");
	//} while (_gen_iteration());
	while (_gen_iteration());
}
bool Wfc::_gen_iteration() {
	// Do a copy
	//Potential ret = old_potential;

	auto to_collapse_pos = _rand_pos_w_least_entropy();
	if (!to_collapse_pos) {
		return false;
	}
	auto& to_collapse
		= _potential.at(to_collapse_pos->y).at(to_collapse_pos->x);

	// The below commented-out `if` statement should never occur because we
	// do this check in `_rand_pos_w_least_entropy()`.
	//if (to_collapse.size() == 0) {
	//	return std::nullopt;
	//}

	//const std::vector<Vec2<size_t>>& non_zero_pos
	//	= find_true(to_collapse);
	const CollapseTemps ct = _calc_collapse_temps(*to_collapse_pos);
	Ddist ddist(ct.full_weight_darr.begin(), ct.full_weight_darr.end());
	to_collapse.clear();
	const auto rng_val = ddist(_rng);
	//const u32* tile = &ct.tile_darr.at(rng_val);
	//to_collapse.at(ct.tile_darr.at(rng_val)) = true;
	to_collapse.insert(ct.tile_darr.at(rng_val));
	//printout("testificate\n");
	_propagate(*to_collapse_pos);
	return true;
}
auto Wfc::_calc_collapse_temps(
	//const Vec2<size_t>& pos
	//const PotElem& pot_elem
	const Vec2<size_t>& pos
) const -> CollapseTemps {
	//WeightsAndTileIds ret;
	CollapseTemps ret;
	ret.pos = pos;
	const PotElem& pot_elem = potential().at(pos.y).at(pos.x);

	//std::vector<double> full_weight_darr;
	//std::unordered_map<u32, size_t> tid_umap;
	if (size_t i=0; true) {
		for (const auto& item: pot_elem) {
			//ret.full_weight_darr.push_back(tprops_umap().at(item.first).weight);
			//if (item.second.valid) 
			//if (item.second)
			//{
				//ret.full_weight_darr.push_back(full_weight_umap().at(item.first));
				ret.full_weight_darr.push_back(full_weight_umap().at(item));
				//ret.tile_darr.push_back(item.first);
				ret.tile_darr.push_back(item);
				//ret.tid_umap.insert(std::pair(item.first, i));
				ret.tid_umap.insert(std::pair(item, i));
				++i;
			//}
		}
	}

	return ret;
}
std::optional<Vec2<size_t>> Wfc::_rand_pos_w_least_entropy() {
	std::optional<double> entropy = std::nullopt;
	std::vector<Vec2<size_t>> lowest_entropy_darr;

	Vec2<size_t> pos;
	//printout("Wfc::_rand_pos_w_least_entropy(): computing...\n");
	for (pos.y=0; pos.y<size_2d().y; ++pos.y) {
		for (pos.x=0; pos.x<size_2d().x; ++pos.x) {
			const auto& tiles = potential().at(pos.y).at(pos.x);
			//const CollapseTemps ct = _calc_collapse_temps(pos);
			//if (ct.tile_darr.size() == 0)
			if (tiles.size() == 0) {
				// We've failed the generation at this point.
				//_dbg_print();
				//sleep(1);
				throw std::runtime_error(sconcat
					("Wfc::_rand_pos_w_least_entropy(): ",
					"potential at position ", pos, " has zero tiles!"));
			}
			//else if (ct.tile_darr.size() == 1)
			else if (tiles.size() == 1) {
				//printout("tile_found ");
				continue;
			}
			std::vector<double> temp_weight_darr, temp_log_weight_darr;

			double temp_entropy = 0.0;
			//for (const auto& tile: tiles)
			//for (const auto& weight: ct.full_weight_darr)
			for (const auto& tile: tiles) {
				//const double& tile_weight = full_weight_umap().at(tile.first);
				const double& weight = full_weight_umap().at(tile);
				temp_entropy -= weight * std::log(weight);
			}
			//temp_entropy = -temp_entropy;
			//printout(temp_entropy, " ");
			if (lowest_entropy_darr.size() == 0) {
				entropy = temp_entropy;
				lowest_entropy_darr.push_back(pos);
			} else { // if (lowest_entropy_darr.size() > 0)
				if (temp_entropy < *entropy) {
					entropy = temp_entropy;
					lowest_entropy_darr.clear();
				}
				lowest_entropy_darr.push_back(pos);
			}
		}
		//printout("\n");
	}
	if (entropy) {
		// Break ties at random
		//printout("Wfc::_rand_pos_w_least_entropy(): ");
		//for (const auto& entropy: lowest_entropy_darr) {
		//	printout(entropy, " ");
		//}
		//printout("\n");
		return lowest_entropy_darr.at
			(rng_run<size_t>(_rng,
				size_t(0), lowest_entropy_darr.size() - 1));
	} else {
		//printout("Wfc::_rand_pos_w_least_entropy(): ",
		//	"Returning `std::nullopt`.\n");
		return std::nullopt;
	}
	//std::sort(entropy_darr.begin(), entropy_darr.end());
}
void Wfc::_propagate(
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

		if (const Vec2<size_t>& pos=needs_update.front(); true)
		{
			needs_update.pop();
			const std::vector<Neighbor>& neighbors = _neighbors(pos);
			for (const auto& neighbor: neighbors) {
				_add_constraint(pos, neighbor, needs_update);
			}
		}
	}
}
void Wfc::_add_constraint(
	const Vec2<size_t>& pos,

	// `neighbor` stores the incoming `Dir`, and the outgoing `Dir` can be
	// computed with `reverse()`
	const Neighbor& neighbor,
	//const CollapseTemps& ct
	//std::stack<Vec2<size_t>>& needs_update
	std::queue<Vec2<size_t>>& needs_update
) {
	PotElem& nb_pot_elem
		= _potential.at(neighbor.pos.y).at(neighbor.pos.x);
	const PotElem& tiles = potential().at(pos.y).at(pos.x);

	PotElem to_erase_uset;

	for (const u32& other_tile: nb_pot_elem) {
		bool found = false;
		//for (const u32& curr_tile: ct.tile_darr)
		for (const u32& curr_tile: tiles) {
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
	for (const u32& to_erase_tile: to_erase_uset) {
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
//void Wfc::_dbg_print() const {
//	Vec2<size_t> pos;
//	for (pos.y=0; pos.y<size_2d().y; ++pos.y) {
//		//printout(pos.y, ": ");
//		for (pos.x=0; pos.x<size_2d().x; ++pos.x) {
//			const auto& pot_elem = potential().at(pos.y).at(pos.x);
//			if (pot_elem.size() == 1) {
//				printout(*pot_elem.begin());
//			} else {
//				printout(pot_elem.size());
//			}
//		}
//		printout("\n");
//	}
//}
//--------
} // namespace wfc

