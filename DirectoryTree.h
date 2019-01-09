#include "File.h"
#include <map>
#include <string>
#include <vector>

class DirectoryTree {
	std::multimap<__ino_t, File *> filesbyinode;
	std::multimap<fsize_t, File *> filesbysize;
	std::map<std::string, File *> filesbyrelativepath;

	void scan(std::string path);
	void AddFile(File *file);

	public:
	std::string root;
	int id;
	static std::vector<DirectoryTree *> trees;

	//! build a directory tree starting at path
	/*!
			Contains multimaps of all files by inode and size
	*/
	DirectoryTree(const std::string &path);

	//! Print all Files by inode
	/*!
			...for debuggering
	*/
	void PrintByInode(void);

	//! Print all files by size
	/*!
			...for debugging
	*/
	void PrintBySize(void);

	//! Print all files by relativepath
	/*!
			...for debugging
	*/
	void PrintByRelativepath(void);

	//! cleanup your mess (RAII)
	/*!
			smart pointers would obviate this
	*/
	~DirectoryTree();
};
