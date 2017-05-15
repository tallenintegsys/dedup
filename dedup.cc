#include <dirent.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <string>
#include <boost/multiprecision/cpp_int.hpp>
#include <experimental/filesystem>
#include <map>

typedef uint32_t inode_t;
typedef uint64_t filesize_t;

class File {

	public:
		inode_t inode;
		std::string name;
		filesize_t size;
		boost::multiprecision::uint128_t md5;
		boost::multiprecision::uint512_t sha;
		static std::map<inode_t, File *> uk_inode;
		static std::multimap<filesize_t, inode_t> cx_size;

		File(inode_t i, std::string n, filesize_t s) {
			inode = i;
			name = n;
			size = s;
			uk_inode.insert(std::pair<inode_t, File *>(inode, this));
			cx_size.insert(std::pair<filesize_t, inode_t>(size, inode)); //XXX should this just store File pointers instead of inodes?
		}

		File(const std::string &filename, const std::string &path) {
			std::string p(path);
			p += std::string("/") += filename;
			struct stat sb;
			std::cout << "add file: " << p << std::endl;

			if (stat(p.c_str(), &sb) == -1) {
				perror("stat");
				exit(EXIT_FAILURE);
			}

			new File(sb.st_ino, p, sb.st_size);

		}
};

std::map<inode_t, File *> File::uk_inode;
std::multimap<filesize_t, inode_t> File::cx_size;

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
			std::cout << "dir:  " << de[n]->d_name << std::endl;
			std::string p(path);
			p+=std::string("/")+=std::string(de[n]->d_name);
			statdir(p);
		}
		if (de[n]->d_type == DT_REG) {
			//regular file
			std::string filename(de[n]->d_name);
			std::cout << "file:  " << filename << std::endl;
			new File(filename, path);
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
		std::cout << ff->inode << "\t   " << ff->size << "\t   " << ff->name << std::endl;
	}

	std::cout << std::endl << "By size:"<< std::endl;
	for (auto f : File::cx_size) {
		inode_t i = f.second;
		File *ff = File::uk_inode[i];
		std::cout << ff->inode << "\t   " << ff->size << "\t   " << ff->name << std::endl;
	}

	exit(EXIT_SUCCESS);
}

