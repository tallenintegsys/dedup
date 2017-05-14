#include <dirent.h>
#include <string>
#include <boost/multiprecision/cpp_int.hpp>
#include <experimental/filesystem>
#include <map>


typedef uint32_t inode_t;
typedef uint64_t filesize_t;
std::map<inode_t, File *> uk_inode;
std::multimap<filesize_t, inode_t> cx_size;


class File {
	public:
		inode_t inode;
		std::string name;
		filesize_t size;
		boost::multiprecision::uint128_t md5;
		boost::multiprecision::uint512_t sha;

		File(inode_t i, std::string n, filesize_t s) {
			inode = i;
			name = n;
			size = s;
			uk_inode.insert(std::pair<inode_t, File *>(inode, this));
			cx_size.insert(std::pair<filesize_t, inode_t>(size, inode));

		}
};

int main(int argv, char **argc) {


	File *f1 = new File(6, "file6", 226);
	File *f2 = new File(2, "file2", 122);
	File *f3 = new File(3, "file3", 123);
	File *f4 = new File(1, "file1", 121);
	File *f5 = new File(4, "file4", 124);
	File *f6 = new File(5, "file5", 125);


	struct dirent **namelist;
	int n;

	n = scandir(".", &namelist, NULL, alphasort);
	if (n < 0)
		perror("scandir");
	else {
		while (n--) {
			std::cout << namelist[n]->d_name << std::endl;
	//		namelist[n]->d_name

			free(namelist[n]);
		}
		free(namelist);
	}


}

