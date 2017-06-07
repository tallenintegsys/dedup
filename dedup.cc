#include <string>
#include <unistd.h>
#include <getopt.h>
#include "RootDirectory.h"

static struct option const long_options[] =
{
  {"notyet", no_argument, NULL, 'n'},
  {"temp", no_argument, NULL, 't'},
  {NULL, 0, NULL, 0}
};

void printsha(unsigned int *sha) {
	for(int i = 0; i < 64 ; i++) {
		if (i == 16) {
			i+=32;
			printf("...");
		}
		printf("%02x", sha[i]);
	}
	printf("\n");
}

int main(int argc, char **argv) {
	while (true)
	{
		int oi = -1;
		int c = getopt_long (argc, argv, "nt", long_options, &oi);
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
		}
	}

	for (int i = optind; i < argc ; i++) {
		//std::cout << argv[i] << std::endl;
		new RootDirectory(std::string(argv[i]));
	}

	//XXX test code, print the stuff out
	for (RootDirectory *rd : RootDirectory::rootdirectories) {
		std::cout << rd->path << std::endl;
		rd->PrintByInode();
		rd->PrintBySize();
		rd->PrintByRelativepath();
	}

	exit(EXIT_SUCCESS);
}

