#include <dirent.h>
#include <fcntl.h>
#include <iomanip>
#include <iostream>
#include <map>
#include <openssl/evp.h>
#include <string.h>
#include <string>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

typedef off_t fsize_t;

//!  Relevent data about each file in the tree
/*!
		This class holds everything related to the files in the tree.
		SHA512's are calculated for files whose file sizes don't make
		it obvious that they are distinct.
*/
class File {
	public:
	__ino_t inode;
	std::string name;
	std::string relname;
	fsize_t size;
	unsigned char *sha = NULL; /*!< SHA512 for the file */
	int hardlinks = 0;
	bool dup = false; //	XXX testing

	//! populates various variables
	/*!
			Always populated inode, name, relativepath, and
			size. adds an entry to the inode and size lookup
			containers
	*/
	File(const std::string &);
	auto operator==(File &) -> bool;
	void link(File *);
	auto isHardlink(File *) -> bool;

	private:
	void calc_sha();

	friend auto operator<<(std::ostream &, const File &) -> std::ostream &;
};
