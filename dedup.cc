#include "FileDB.h"
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

void printsha(unsigned int *sha) {
	for (int i = 0; i < 64; i++) {
		if (i == 16) {
			i += 32;
			printf("...");
		}
		printf("%02x", sha[i]);
	}
	printf("\n");
}

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
		//std::cout << i << ":   " << argv[i] << std::endl;
	}

	FileDB filedb;
	for (auto path : paths) {
		//std::cout << path << "\n";
		// recursive_directory_iterator
		for (auto const &dir_entry : std::filesystem::recursive_directory_iterator{path}) {
			if (dir_entry.is_regular_file()) {
				//std::cout << dir_entry << '\n';
				filedb.addFile(dir_entry);
			}
		}
	}

	//	filedb.printBySHA();
	filedb.printDups();
	exit(EXIT_SUCCESS);
}
