#include <dirent.h>
#include <string>
#include <boost/multiprecision/cpp_int.hpp>
#include <experimental/filesystem>
#include <map>


typedef uint32_t inode_t;
typedef uint64_t filesize_t;

struct file {
	inode_t inode;
	std::string name;
	filesize_t size;
	boost::multiprecision::uint128_t md5;
	boost::multiprecision::uint512_t sha;
};

std::map<inode_t, file *> uk_inode;
std::multimap<filesize_t, inode_t> cx_size;


void newFile(inode_t i, std::string n, filesize_t s) {
	file *f = new file;
	f->inode = i;
	f->name = n;
	f->size = s;
	uk_inode.insert(std::pair<inode_t, file *>(f->inode, f));
	cx_size.insert(std::pair<filesize_t, inode_t>(f->size, f->inode));

}

int main(int argv, char **argc) {


	newFile(6, "file6", 226);
	newFile(2, "file2", 122);
	newFile(3, "file3", 123);
	newFile(1, "file1", 121);
	newFile(4, "file4", 124);
	newFile(5, "file5", 125);


	struct dirent **namelist;
	int n;

	n = scandir(".", &namelist, NULL, alphasort);
	if (n < 0)
		perror("scandir");
	else {
		while (n--) {
			std::cout << namelist[n]->d_name << std::endl;
			free(namelist[n]);
		}
		free(namelist);
	}


}

