#include "metatile_class.hpp"

namespace wfc {
//--------
Metatile::Metatile() {
}
Metatile::Metatile(size_t s_dim)
	: _data(s_dim, std::vector<size_t>(s_dim, 0)) {
}
Metatile::~Metatile() {
}

Metatile& Metatile::reflect_x() {
	Vec2<size_t> pos;
	for (pos.y=0; pos.y<dim(); ++pos.y) {
		std::reverse(_data.at(pos.y).begin(), _data.at(pos.y).end());
		//std::vector<size_t> temp(dim(), 0);
		//for (pos.x=0; pos.x<dim(); ++pos.x) {
		//	temp.at(dim() - pos.x - 1) = at(pos);
		//}
		//for (pos.x=0; pos.x<dim(); ++pos.x) {
		//	at(pos) = temp.at(pos.x);
		//}
	}
	return *this;
}
Metatile& Metatile::reflect_y() {
	Vec2<size_t> pos;
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
	return *this;
}
Metatile& Metatile::transpose() {
	// Metatiles are assumed to be small, so we can do this with a
	// temporary `Metatile`
	Metatile temp(dim());

	Vec2<size_t> pos;
	for (pos.y=0; pos.y<dim(); ++pos.y) {
		for (pos.x=0; pos.x<dim(); ++pos.x) {
			temp.at({pos.y, pos.x}) = at(pos);
		}
	}
	*this = std::move(temp);

	return *this;
}
Metatile& Metatile::rotate_p90() {
	//Metatile ret(to_rotate.size(),
	//	std::vector<size_t>(to_rotate.front().size(), 0));

	//return reflect_x().reflect_y().reflect_x();
	//return reflect_y().reflect_x().reflect_y();
	//return *this;
	//return reflect_x().reflect_y();
	return transpose().reflect_x();
	//Metatile temp(dim());
	//Vec2<size_t> pos;

	//for (pos.y=0; pos.y<dim(); ++pos.y) {
	//	for (pos.x=0; pos.x<dim(); ++pos.x) {
	//	}
	//}

	//*this = std::move(temp);
	//return *this;
}
//--------
} // namespace wfc
