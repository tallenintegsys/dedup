#include <string>
#include <unistd.h>
#include <getopt.h>
#include "RootDirectory.h"

static struct option const long_options[] =
{
  {"all", no_argument, NULL, 'a'},
  {"escape", no_argument, NULL, 'b'},
  {"directory", no_argument, NULL, 'd'},
  {"dired", no_argument, NULL, 'D'},
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
		int c = getopt_long (argc, argv,
				"abcdD",
				long_options, &oi);
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
	const std::string path(argv[1]);
	RootDirectory rd1 = RootDirectory(path);

	rd1.PrintByInode();
	rd1.PrintBySize();

	exit(EXIT_SUCCESS);
}

