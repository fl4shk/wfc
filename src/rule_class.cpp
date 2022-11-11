#include "rule_class.hpp"

namespace wfc {
//--------
Metatile::Metatile(size_t s_dim)
	: _data(s_dim, std::vector<size_t>(s_dim, 0)) {
}
Metatile& Metatile::rotate_p90() {
	//Metatile ret(to_rotate.size(),
	//	std::vector<size_t>(to_rotate.front().size(), 0));

	// Note that the first two `for` loops transpose the matrix
	Vec2<size_t> pos;
	for (pos.y=0; pos.y<dim(); ++pos.y) {
		std::reverse(_data.at(pos.y).begin(), _data.at(pos.y).end());
	}
	for (pos.x=0; pos.x<dim(); ++pos.x) {
		std::vector<size_t> temp(dim(), 0);
		// reverse columns with a temporary `std::vector`.
		for (pos.y=0; pos.y<dim(); ++pos.y) {
			temp.at(dim() - pos.y - 1) = at(pos);
		}
		for (pos.y=0; pos.y<dim(); ++pos.y) {
			at(pos) = temp.at(pos.y);
		}
	}
	for (pos.y=0; pos.y<dim(); ++pos.y) {
		std::reverse(_data.at(pos.y).begin(), _data.at(pos.y).end());
	}

	return *this;
}
//--------
} // namespace wfc
