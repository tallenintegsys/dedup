#include <sys/types.h>
#include <string.h>
#include <string>
#include "File.h"

void statdir(const std::string& path) {
	struct dirent **de;

	int n = scandirat(AT_FDCWD,path.c_str(), &de, NULL, alphasort); 
	if (n == -1)
		return;

	while(n--) {
		if (!strcmp(de[n]->d_name, "..") || !strcmp(de[n]->d_name, "."))
			continue;

		if (de[n]->d_type == DT_DIR) {
			continue; //XXX just testing for now don't descend the tree
			//directory
			std::string p(path);
			p+=std::string("/")+=std::string(de[n]->d_name);
			statdir(p); //recurse
		}
		if (de[n]->d_type == DT_REG) {
			//regular file
			std::string filename(de[n]->d_name);
			std::cout << filename << std::endl;
			new File(path, filename);
		}
	}
}

int main(int argv, char **argc) {

	if (argv != 2) {
		perror("Insuficient arguments");
		exit(EXIT_FAILURE);
	}
	statdir(argc[1]);

	std::cout << "By inode:"<< std::endl;
	for (auto pf : File::uk_inode) {
		File *f = pf.second;
		std::cout<<f->inode<<"\t "<<f->size<<"\t "<<"\t "<<f->name <<"\t ";
		if (f->sha) {
			for(int i = 0; i < 64 ; i++)
				printf("%02x", f->sha[i]);
			printf("\n");

		} else {
			std::cout << std::endl;
		}
	}

	std::cout << std::endl << "By size:"<< std::endl;
	for (auto pf : File::cx_size) {
		auto f = pf.second;
		std::cout<<f->inode<<"\t "<<f->size<<"\t "<<"\t "<<f->name<<"\t ";
		if (f->sha) {
			for(int i = 0; i < 64 ; i++)
				printf("%02x", f->sha[i]);
			printf("\n");

		} else {
			std::cout << std::endl;
		}
	}

	exit(EXIT_SUCCESS);
}

