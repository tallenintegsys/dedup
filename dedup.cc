#include <string>
#include "File.h"

void statdir(const std::string& path) {
	struct dirent **de;

	int n = scandirat(AT_FDCWD, path.c_str(), &de, NULL, alphasort);
	if (n == -1)
		return;

	while(n--) {
		if (!strcmp(de[n]->d_name, "..") || !strcmp(de[n]->d_name, "."))
			continue;

		if (de[n]->d_type == DT_DIR) {
			//continue; //XXX just testing for now don't descend the tree
			//directory
			std::string p(path);
			p+=std::string("/")+=std::string(de[n]->d_name);
			statdir(p); //recurse
		}
		if (de[n]->d_type == DT_REG) {
			//regular file
			std::string filename(de[n]->d_name);
			//std::cout << filename << std::endl;
			new File(path, filename); //this is actually not a memory leak

		}
	}
	free(de);
}

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
	statdir(argc[1]);

	exit(EXIT_SUCCESS);
}

