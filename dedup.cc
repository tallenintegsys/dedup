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
		{"notyet", no_argument, NULL, 'n'}, {"relink", no_argument, NULL, 'r'}, {NULL, 0, NULL, 0}};

int main(int argc, char **argv) {
	bool relink = false;
	while (true) {
		int oi = -1;
		int c = getopt_long(argc, argv, "rnt", long_options, &oi);
		if (c == -1)
			break;

		switch (c) {
		case 'n':
			break;
		case 'r':
			relink = true;
			break;
		default: /* '?' */
			std::cerr << "Usage: " << argv[0] << "[OPTION...] FILE... \n";
			std::cerr << "OPTIONS:\n";
			std::cerr << "-r --relink   unlink (delete) duplicate files and hard link to one inode\n";
			std::cerr << "By default lists duplicate files\n";
			exit(EXIT_FAILURE);
		} // switch
	}  // while

	std::list<std::filesystem::path> paths;
	for (int i = optind; i < argc; i++)
		paths.push_back(std::filesystem::path(std::string(argv[i])));

	FileDB2 filedb(relink);
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
	filedb.printDups();
	//	filedb.printFilesWithSameSha();
	//	filedb.printFilesWithSameShaDifferentInode();

	return EXIT_SUCCESS;
}
