#include "misc_includes.hpp"
#include "wfc_class.hpp"

int main(int argc, char** argv) {
	//--------
	//if (argc < 4 || argc > 5) {
	//	printerr("Usage 1: ", argv[0], " input_tiles_file width height\n");
	//	printerr("Usage 2: ", argv[0],
	//		" input_tiles_file width height rng_seed\n");
	//	exit(1);
	//}
	ArgParser ap;
	ap.add("--input-file", "-i", HasArg::Req, true, true);
	ap.add("--width", "-w", HasArg::Req, true, true);
	ap.add("--height", "-h", HasArg::Req, true, true);
	ap.add("--tile-dim", "-d", HasArg::Req, false, true);
	ap.add("--rotate", "-r", HasArg::None, false, true);
	ap.add("--overlap", "-o", HasArg::None, false, true);
	ap.add("--seed", "-s", HasArg::Req, false, true);
	if (const auto& ap_ret=ap.parse(argc, argv); ap_ret.fail()) {
		//printerr("Error: invalid arguments\n");
		printerr(ap.help_msg(argc, argv));
		exit(1);
	}

	std::vector<std::vector<u32>> input_tiles;
	if (
		std::fstream file(ap.at("-i", 0).val, std::ios_base::in);
		file.is_open()
	) {
		while (!file.eof()) {
			std::string line;
			std::getline(file, line);
			if (file.eof() && line.size() == 0) {
			} else if (
				input_tiles.size() > 0
				&& input_tiles.back().size() != line.size()
			) {
				printerr
					("Input file ",
					"\"", ap.at("-i", 0).val, "\" "
					"has difference in line sizes ",
					"(found first instance at line numbers ",
						"[", input_tiles.size(), ", ",
							(input_tiles.size() + size_t(1)), "])"
					"\n");
				exit(1);
			} else if (line.size() == 0) {
				printerr("Input file ",
					"\"", ap.at("-i", 0).val, "\" "
					"has line of size 0 ",
					"(found first instance at line number ",
						input_tiles.size(), ")",
					"\n");
				exit(1);
			} else {
				//input_tiles.push_back(std::move(line));
				std::vector<u32> darr;
				for (const auto& c: line) {
					darr.push_back(static_cast<u32>(c));
				}
				input_tiles.push_back(std::move(darr));
			}
		}
	} else {
		printerr("Couldn't open input file ",
			"\"", ap.at("-i", 0).val, "\" ",
			"for reading.\n");
		exit(1);
	}
	//printout("Read in this these `input_tiles`\n");
	//for (size_t j=0; j<input_tiles.size(); ++j) {
	//	printout(j, ": ", input_tiles.at(j), "\n");
	//}

	static constexpr size_t
		MIN_OUTPUT_DIM = 1,
		MAX_OUTPUT_DIM = 32;

	Vec2<size_t> size_2d;
	inv_sconcat(ap.at("-w", 0).val, size_2d.x);
	if (size_2d.x < MIN_OUTPUT_DIM || size_2d.x > MAX_OUTPUT_DIM) {
		printerr("`width` (", size_2d.x, ") must be in the ",
			"range [", MIN_OUTPUT_DIM, ", ", MAX_OUTPUT_DIM, "]\n");
	}
	inv_sconcat(ap.at("-h", 0).val, size_2d.y);
	if (size_2d.y < MIN_OUTPUT_DIM || size_2d.y > MAX_OUTPUT_DIM) {
		printerr("`height` (", size_2d.y, ") must be in the ",
			"range [", MIN_OUTPUT_DIM, ", ", MAX_OUTPUT_DIM, "]\n");
	}

	Vec2<size_t> mt_size_2d(1, 1);
	if (ap.has_opts("-d")) {
		size_t tile_dim;
		inv_sconcat(ap.at("-d", 0).val, tile_dim);
		mt_size_2d = {.x=tile_dim, .y=tile_dim};
	}

	const bool
		rotate = ap.has_opts("-r"),
		overlap = ap.has_opts("-o");

	u64 rng_seed;

	if (!ap.has_opts("-s")) {
		rng_seed = get_hrc_now_rng_seed();
	} else {
		inv_sconcat(ap.at("-s", 0).val, rng_seed);
	}

	wfc::Wfc the_wfc
		(size_2d, mt_size_2d,
		input_tiles,
		rotate, overlap,
		rng_seed);
	for (size_t j=0; j<the_wfc.potential().size(); ++j) {
		const auto& row = the_wfc.potential().at(j);
		//printout(j, ": ");
		for (size_t i=0; i<row.size(); ++i) {
			const auto& uset = row.at(i);
			if (uset.size() != 1) {
				printerr("main(): Eek! ",
					"{", i, ", ", j, "}: ", uset.size(), "\n");
				exit(1);
			}
			//printout(i32(uset.begin()->first));
			//printout(uset.begin()->first);
			printout(static_cast<char>(*uset.begin()));
			//for (const auto& item: umap) {
			//	if (item.second) {
			//		printout(item.first);
			//		break;
			//	}
			//}
		}
		printout("\n");
	}

	return 0;
}
