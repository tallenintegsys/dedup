#include <string>
#include "RootDirectory.h"

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

int main(int argv, char **argc) {

	if (argv != 2) {
		perror("Insuficient arguments");
		exit(EXIT_FAILURE);
	}
	const std::string path(argc[1]);
	RootDirectory rd1 = RootDirectory(path);

	rd1.PrintByInode();
	rd1.PrintBySize();

	exit(EXIT_SUCCESS);
}

