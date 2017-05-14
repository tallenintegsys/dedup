#include <dirent.h>
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
			cx_size.insert(std::pair<filesize_t, inode_t>(size, inode));
		}
};
std::map<inode_t, File *> File::uk_inode;
std::multimap<filesize_t, inode_t> File::cx_size;

int main(int argv, char **argc) {

	if (argv != 2)
		return -1;


	struct dirent **namelist;
	int n;

	n = scandir(argc[1], &namelist, NULL, alphasort);
	if (n < 0)
		perror("scandir");
	else {
		while (n--) {
			std::cout << namelist[n]->d_name << std::endl;
			new File(namelist[n]->d_ino, namelist[n]->d_name, namelist[n]->d_off);
			free(namelist[n]);
		}
		free(namelist);
	}
	for (auto f : File::uk_inode) {
		File *ff = f.second;
		std::cout << ff->inode << "   " << ff->size << "   " << ff->name << std::endl;
	}

}

