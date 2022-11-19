#include "misc_includes.hpp"
#include "wfc_class.hpp"

int main(int argc, char** argv) {
	//--------
	//wfc::Metatile mt(3);
	//for (size_t j=0; j<mt.dim(); ++j) {
	//	for (size_t i=0; i<mt.dim(); ++i) {
	//		mt.at({.x=i, .y=j}) = '0' + size_t((j * mt.dim()) + i);
	//	}
	//}
	//for (size_t j=0; j<mt.dim(); ++j) {
	//	for (size_t i=0; i<mt.dim(); ++i) {
	//		printout(static_cast<char>(mt.at({i, j})));
	//	}
	//	printout("\n");
	//}
	//printout("\n");
	//mt.rotate_p90();
	//for (size_t j=0; j<mt.dim(); ++j) {
	//	for (size_t i=0; i<mt.dim(); ++i) {
	//		printout(static_cast<char>(mt.at({i, j})));
	//	}
	//	printout("\n");
	//}

	//return 0;
	//--------
	//if (argc < 4 || argc > 5) {
	//	printerr("Usage 1: ", argv[0], " input_tiles_file width height\n");
	//	printerr("Usage 2: ", argv[0],
	//		" input_tiles_file width height rng_seed\n");
	//	exit(1);
	//}
	ArgParser ap;
	ap
		.add_singleton("--input-file", "-i", HasArg::Req, true,
			sconcat
				("\n\tName of the input file containing a 2D array of ",
				"characters to learn from.\n\t"))

		.add_singleton("--chunk-width", "-w", HasArg::Req, true, "")
		.add_singleton("--chunk-height", "-h", HasArg::Req, true, "")
		.add_singleton("--num-chunks-x", "-W", HasArg::Req, false,
			"\n\tNumber of chunks in the x dimension. Defaults to 1.\n\t")
		.add_singleton("--num-chunks-y", "-H", HasArg::Req, false,
			"\n\tNumber of chunks in the y dimension. Defaults to 1.\n\t")

		.add_singleton("--metatile-dim", "-d", HasArg::Req, false,
			sconcat
				("\n\tWidth AND height of multi-tile objects ",
					"(metatiles). ",
				"\n\tThis option only takes effect with --overlap.",
				"\n\tDefaults to 1.\n\t"))
		.add_singleton("--debug-print", "-p", HasArg::None, false,
			"\n\tShow progress with debug printing\n\t")

		.add_singleton("--backtrack", "-b", HasArg::None, false,
			sconcat
				("\n\tPerform backtracking instead of restarting upon a "
				"propagate fail.\n\t"))
		.add_singleton("--overlap", "-o", HasArg::None, false,
			"\n\tUse the overlapping model, which involves metatiles.\n\t")
		.add_singleton("--rotate", "-r", HasArg::None, false,
			"\n\tRotate metatiles.\n\t")
		.add_singleton("--reflect", "-R", HasArg::None, false,
			"\n\tReflect metatiles.\n\t")
		.add_singleton("--seed", "-s", HasArg::Req, false,
			"\n\tRNG seed. Mainly useful for debugging the program.\n\t");
	if (
		const auto& ap_ret=ap.parse(argc, argv);
		ap_ret.fail() || ap_ret.index != argc || argc == 1
	) {
		//printerr("Error: invalid arguments\n");
		printerr(ap.help_msg(argc, argv));
		exit(1);
	}

	std::vector<std::vector<size_t>> input_tiles;
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
				std::vector<size_t> darr;
				for (const auto& c: line) {
					darr.push_back(static_cast<size_t>(c));
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
		MIN_CHUNK_DIM = 1,
		MAX_CHUNK_DIM
			= 32;
			//= 64;

	Vec2<size_t> chunk_size_2d;
	inv_sconcat(ap.at("-w", 0).val, chunk_size_2d.x);
	if (chunk_size_2d.x < MIN_CHUNK_DIM || chunk_size_2d.x > MAX_CHUNK_DIM) {
		printerr("`chunk-width` (", chunk_size_2d.x, ") must be in the ",
			"range [", MIN_CHUNK_DIM, ", ", MAX_CHUNK_DIM, "].\n");
		exit(1);
	}
	inv_sconcat(ap.at("-h", 0).val, chunk_size_2d.y);
	if (chunk_size_2d.y < MIN_CHUNK_DIM || chunk_size_2d.y > MAX_CHUNK_DIM) {
		printerr("`chunk-height` (", chunk_size_2d.y, ") must be in the ",
			"range [", MIN_CHUNK_DIM, ", ", MAX_CHUNK_DIM, "].\n");
		exit(1);
	}

	static constexpr size_t
		MIN_NUM_CHUNKS_DIM = 1,
		MAX_NUM_CHUNKS_DIM
			//= 4;
			= 10;
	Vec2<size_t> num_chunks_2d(1, 1);
	if (ap.has_opts("--num-chunks-x")) {
		inv_sconcat(ap.at("--num-chunks-x", 0).val, num_chunks_2d.x);
		if (
			num_chunks_2d.x < MIN_NUM_CHUNKS_DIM
			|| num_chunks_2d.x > MAX_NUM_CHUNKS_DIM
		) {
			printerr("`num-chunks-x` (", num_chunks_2d.x, ") must be in ",
				"the range ",
				"[", MIN_NUM_CHUNKS_DIM, ", ", MAX_NUM_CHUNKS_DIM, "].\n");
			exit(1);
		}
	}
	if (ap.has_opts("--num-chunks-y")) {
		inv_sconcat(ap.at("--num-chunks-y", 0).val, num_chunks_2d.y);
		if (
			num_chunks_2d.y < MIN_NUM_CHUNKS_DIM
			|| num_chunks_2d.y > MAX_NUM_CHUNKS_DIM
		) {
			printerr("`num-chunks-y` (", num_chunks_2d.y, ") must be in ",
				"the range ",
				"[", MIN_NUM_CHUNKS_DIM, ", ", MAX_NUM_CHUNKS_DIM, "].\n");
			exit(1);
		}
	}

	//Vec2<size_t> mt_size_2d(1, 1);
	size_t mt_dim = 1;
	if (ap.has_opts("--metatile-dim")) {
		inv_sconcat(ap.at("-d", 0).val, mt_dim);
	}

	const bool
		opt_debug_print = ap.has_opts("--debug-print"),
		opt_backtrack = ap.has_opts("--backtrack"),
		opt_overlap = ap.has_opts("--overlap"),
		opt_rotate = ap.has_opts("--rotate"),
		opt_reflect = ap.has_opts("--reflect");

	u64 rng_seed;

	if (!ap.has_opts("-s")) {
		rng_seed = get_hrc_now_rng_seed();
	} else {
		inv_sconcat(ap.at("-s", 0).val, rng_seed);
	}

	wfc::Wfc the_wfc
		(chunk_size_2d, num_chunks_2d,
		mt_dim,
		//input_tiles,
		opt_debug_print,
		opt_backtrack, opt_overlap, opt_rotate, opt_reflect,
		rng_seed);
	the_wfc.learn(input_tiles);
	the_wfc.gen();
	if (opt_debug_print) {
		printout("Result:\n");
	}
	for (size_t j=0; j<the_wfc.result().size(); ++j) {
		const auto& row = the_wfc.result().at(j);
		//printout(j, ": ");
		for (size_t i=0; i<row.size(); ++i) {
			const auto& pot_elem = row.at(i);
			//if (uset.size() != 1)
			if (pot_elem.num_active() != 1)
			{
				//printerr("main(): Eek! ",
				//	"{", i, ", ", j, "}: ", uset.size(), "\n");
				printerr("main(): Eek! ",
					"{", i, ", ", j, "}: ", pot_elem.num_active(),
					"\n");
				exit(1);
			}
			//printout(i32(uset.begin()->first));
			//printout(uset.begin()->first);
			//printout(static_cast<char>(the_wfc.mt_darr()
			//	.at(*uset.begin()).tl_corner()));
			printout(static_cast<char>(the_wfc.mt_darr()
				.at(*pot_elem.first_set()).tl_corner()));
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
