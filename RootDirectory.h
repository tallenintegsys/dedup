#include <string>
#include <map>
#include <vector>
#include "File.h"

class RootDirectory {
	std::multimap<__ino_t, File *> filesbyinode;
	std::multimap<fsize_t, File *> filesbysize;
	std::map<std::string, File *> filesbyrelativepath;

	void scan(std::string path);
	void AddFile(File *file);

	public:
	std::string root;
	int id;
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

	 //! Print all files by relativepath
	 /*!
	   ...for debugging
	 */
	void PrintByRelativepath (void);

	 //! cleanup your mess (RAII)
	 /*!
	   smart pointers would obviate this
	 */
	~RootDirectory();
};
