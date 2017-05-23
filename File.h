#include <dirent.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string>
#include <boost/multiprecision/cpp_int.hpp>
#include <map>
#include <sys/mman.h>
#include <openssl/evp.h>

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
		std::string relativepath;
		fsize_t size;
		unsigned char *sha = NULL; /*!< SHA512 for the file */ 
		bool hardlink; /*!< it's alread a hardlink */ 
		static std::map<__ino_t, File *> uk_inode; /*< files by inode */
		static std::multimap<fsize_t, File *> cx_size; /*< files by size */
		static std::multimap<__ino_t, File *> cx_hardlinks; //XXX for test only

		 //! populates various variables
		 /*!
		   Always populated inode, name, relativepath, and 
		   size. adds an entry to the inode and size lookup
		   containers
		 */
		File(const std::string &, const std::string &);
	private:
		void calc_sha();
		bool isHardlink(File *);
		void link(File *);
		bool equal (File &);
};
