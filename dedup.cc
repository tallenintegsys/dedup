#include <dirent.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <string>
#include <boost/multiprecision/cpp_int.hpp>
#include <experimental/filesystem>
#include <map>

typedef off_t fsize_t;

class File {

	public:
		__ino_t inode;
		std::string name;
		fsize_t size;
		boost::multiprecision::uint128_t md5;
		boost::multiprecision::uint512_t sha;
		static std::map<ino_t, File *> uk_inode;
		static std::multimap<fsize_t, ino_t> cx_size;

		File(const std::string &path, const std::string &filename) {
			std::string p(path);
			p += std::string("/") += filename;
			struct stat sb;

			if (stat(p.c_str(), &sb) == -1) {
				perror("stat");
				exit(EXIT_FAILURE);
			}
			inode = sb.st_ino;
			size = sb.st_size;

			uk_inode.insert(std::pair<ino_t, File *>(inode, this));
			cx_size.insert(std::pair<fsize_t, ino_t>(size, inode));
			if (cx_size.count(sb.st_size) > 1) {

				// calc the md5 and sha512 for this entrant
				md5 = sb.st_atim.tv_sec; //XXX dummy
				sha = sb.st_ctim.tv_nsec; //XXX dummy
			}
			if (cx_size.count(sb.st_size) == 2) {
				// find the other identically sized file and
				// calc it's md5 and sha512
			}
			// the others files should have already had there md5s
			// and sha512s calculated
		}
};

std::map<ino_t, File *> File::uk_inode;
std::multimap<fsize_t, ino_t> File::cx_size;

void statdir(const std::string& path) {
	struct dirent **de;

	int n = scandirat(AT_FDCWD,path.c_str(), &de, NULL, alphasort); 
	if (n == -1)
		return;

	while(n--) {
		if (!strcmp(de[n]->d_name, "..") || !strcmp(de[n]->d_name, "."))
			continue;
		
		if (de[n]->d_type == DT_DIR) {
			//directory
			std::string p(path);
			p+=std::string("/")+=std::string(de[n]->d_name);
			statdir(p);
		}
		if (de[n]->d_type == DT_REG) {
			//regular file
			std::string filename(de[n]->d_name);
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
	for (auto f : File::uk_inode) {
		File *ff = f.second;
		std::cout<<ff->inode<<"\t "<<ff->size<<"\t "<<"\t "<<ff->md5<<"\t "<<ff->sha<<"\t "<<ff->name << std::endl;
	}

	std::cout << std::endl << "By size:"<< std::endl;
	for (auto f : File::cx_size) {
		__ino_t i = f.second;
		File *ff = File::uk_inode[i];
		std::cout<<ff->inode<<"\t "<<ff->size<<"\t "<<"\t "<<ff->md5<<"\t "<<ff->sha<<"\t "<<ff->name << std::endl;
	}

	exit(EXIT_SUCCESS);
}

