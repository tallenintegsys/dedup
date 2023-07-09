#include "FileDB2.h"
#include <algorithm>
#include <filesystem>
#include <fstream>
#include <getopt.h>
#include <iostream>
#include <list>
#include <string>
#include <unistd.h>

static struct option const long_options[] = {
		{"notyet", no_argument, NULL, 'n'}, {"temp", no_argument, NULL, 't'}, {NULL, 0, NULL, 0}};

int main(int argc, char **argv) {
	while (true) {
		int oi = -1;
		int c = getopt_long(argc, argv, "nt", long_options, &oi);
		if (c == -1)
			break;

		switch (c) {
		case 'n':
			break;
		case 't':
			break;
		default: /* '?' */
			fprintf(stderr, "Usage: %s [-t nsecs] [-n] name\n", argv[0]);
			exit(EXIT_FAILURE);
		} // switch
	}  // while

	std::list<std::filesystem::path> paths;
	for (int i = optind; i < argc; i++) {
		paths.push_back(std::filesystem::path(std::string(argv[i])));
		//	std::cout << i << ":   " << argv[i] << std::endl;
	}

	FileDB2 filedb;
	for (auto path : paths) {
		//	std::cout << path << "\n";
		// 	recursive_directory_iterator
		for (std::filesystem::directory_entry const &dir_entry : std::filesystem::recursive_directory_iterator{path}) {
			if (dir_entry.is_regular_file()) {
				filedb.addFile(dir_entry);
			}
		}
	}

	//	filedb.printBySHA();
	//filedb.printDups();
	filedb.printFilesWithSameSha();
	
	return EXIT_SUCCESS;
}
