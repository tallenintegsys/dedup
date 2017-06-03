#include <dirent.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string>
#include <string.h>
#include <map>
#include <vector>
#include <sys/mman.h>
#include <openssl/evp.h>
#include <iostream>
#include <iomanip>
#include "File.h"

class RootDirectory {
	std::multimap<__ino_t, File *> filesbyinode;
	std::multimap<fsize_t, File *> filesbysize;

	void scan(const std::string &path);
	void AddFile(File *file);

	public:
	std::string path;
	static std::vector<RootDirectory *> rootdirectories;
	 //! build a directory tree starting at path
	 /*!
	   Contains multimaps of all files by inode and size
	 */
	RootDirectory(const std::string &path);

	 //! Print all Files by inode
	 /*!
	   ...for debuggering
	 */
	void PrintByInode (void);

	 //! Print all files by size
	 /*!
	   ...for debugging
	 */
	void PrintBySize (void);

	 //! cleanup your mess (RAII)
	 /*!
	   smart pointers would obviate this
	 */
	~RootDirectory();
};
