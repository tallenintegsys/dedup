#include <dirent.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string>
#include <string.h>
#include <map>
#include <sys/mman.h>
#include <openssl/evp.h>
#include <iostream>
#include <iomanip>

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
	std::string root;
	std::string relpath;
	std::string name;
	std::string fullpath;
	fsize_t size;
	unsigned char *sha = NULL; /*!< SHA512 for the file */ 
	bool hardlink = false; /*!< it's alread a hardlink */
	nlink_t nlink = 0;
	bool dup = false; //XXX testing

	 //! populates various variables
	 /*!
	   Always populated inode, name, relativepath, and 
	   size. adds an entry to the inode and size lookup
	   containers
	 */
	File(const std::string &, const std::string &, const std::string &);
	bool operator== (File &);
	void link(File *);
	bool isHardlink(File *);

	private:
	void calc_sha();
};
