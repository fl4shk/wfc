#include "misc_includes.hpp"
#include "wfc_class.hpp"

int main(int argc, char** argv) {
	//--------
	Vec2<size_t> size_2d;

	if (argc < 4 || argc > 5) {
		printerr("Usage 1: ", argv[0], " input_tiles_file width height\n");
		printerr("Usage 2: ", argv[0],
			" input_tiles_file width height rng_seed\n");
		exit(1);
	}
	//ArgParser ap;
	//ap.add("--ifile", "-i", HasArg::Req);
	//ap.add("--width", "-w", HasArg::Req);
	//ap.add("--height", "-h", HasArg::Req);
	//ap.parse(argc, argv);

	std::vector<std::vector<u32>> input_tiles;
	if (
		std::fstream file(argv[1], std::ios_base::in);
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
					("Input tiles file \"", argv[1], "\" has ",
					"difference in line sizes ",
					"(found first instance at line numbers ",
						"[", input_tiles.size(), ", ",
							(input_tiles.size() + size_t(1)), "])"
					"\n");
				exit(1);
			} else if (line.size() == 0) {
				printerr("Input tiles file \"", argv[1], "\" has line ",
					"of size 0 ",
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
		printerr("Couldn't open input tiles file \"", argv[1], "\" for ",
			"reading.\n");
		exit(1);
	}
	//printout("Read in this these `input_tiles`\n");
	//for (size_t j=0; j<input_tiles.size(); ++j) {
	//	printout(j, ": ", input_tiles.at(j), "\n");
	//}

	for (size_t i=0; i<size_2d.size(); ++i) {
		inv_sconcat(std::string(argv[i + 2]), size_2d.at(i));

		if (size_2d.at(i) < 1 || size_2d.at(i) > 32) {
			if (i == size_2d.IND_X) {
				printerr("`width` (", size_2d.at(i), ") must be in the ",
					"range [1, 32]\n");
			} else if (i == size_2d.IND_Y) {
				printerr("`height` (", size_2d.at(i), ") must be in the ",
					"range [1, 32]\n");
			}
			exit(1);
		}
	}

	u64 seed;

	if (argc == 4) {
		seed = get_hrc_now_rng_seed();
	} else { // if (argc == 5)
		inv_sconcat(std::string(argv[4]), seed);
	}

	wfc::Wfc the_wfc(size_2d, seed, input_tiles);
	for (size_t j=0; j<the_wfc.potential().size(); ++j) {
		const auto& row = the_wfc.potential().at(j);
		printout(j, ": ");
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
