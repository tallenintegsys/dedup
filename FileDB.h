#ifndef _FILEDB_H
#define _FILEDB_H 1

#include "Sha512.h"
#include <filesystem>
#include <map>
#include <set>
#include <string>
#include <unistd.h>
#include <vector>

namespace fs = std::filesystem;

class FileDB {

	class File {
public:
		fs::directory_entry dirent;
		ino_t inode;
		Sha512 sha;
		friend auto operator<<(std::ostream &os, const File &rhs) -> std::ostream & {
			os << std::setfill(' ') << std::setw(30) << std::left << rhs.dirent;
			os << std::setw(20) << rhs.inode;
			os << std::setw(30) << rhs.sha << "\n";
			return os;
		}
	};

	bool relink = false;

	std::multimap<Sha512, const File> filesBySha;
	std::set<Sha512> uniqueShas;

	/// @brief
	/// @param
	/// @return
	Sha512 calcSha(const fs::directory_entry &);

	ino_t getInode(const fs::directory_entry &);

	public:
	//! Construct a new FileDB2
	/*!
		Contains multimaps of all files by inode and size
	*/
	FileDB(bool relink);

	//! add File to containers
	/*!
		thus creating an ad-hoc DB
	*/
	void addFile(const fs::directory_entry &);

	/// @brief
	/// @param
	void printDups(void);

	//! cleanup your mess (RAII)
	/*!
		smart pointers would obviate this
	*/
	~FileDB();
};
#endif // _FILEDB_H