#include <filesystem>
#include <map>
#include <set>
#include <string>
#include <unistd.h>
#include <vector>
#include "Sha512.h"

namespace fs = std::filesystem;

class FileDB2 {

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

	std::multimap<Sha512, const File> filesBySha;
	std::multimap<ino_t, const File> filesByInode;


	/// @brief
	/// @param
	/// @return
	Sha512 calcSha(const fs::directory_entry &);

	ino_t getInode(const fs::directory_entry &);

	bool isShaDup(const Sha512 &);

	bool isInodeDup(const ino_t &);

	/// @brief get all files with the same SHA only if there is more than one
	/// @param  none
	/// @return vector of Files with the same SHA GROUP BY SHA
	std::vector<File> filesWithSameSha(void);

	std::set<Sha512> findDupShas();

	std::vector<FileDB2::File> filesWithSameShaDifferentInode(std::vector<FileDB2::File>);

	public:
	//! Construct a new FileDB2
	/*!
		Contains multimaps of all files by inode and size
	*/
	FileDB2();

	//! add File to containers
	/*!
		thus creating an ad-hoc DB
	*/
	void addFile(const fs::directory_entry &);

	/// @brief
	/// @param
	void printDups(void);

	void printFilesWithSameSha(void);

	void printFilesWithSameShaDifferentInode(void);

	//! cleanup your mess (RAII)
	/*!
		smart pointers would obviate this
	*/
	~FileDB2();
};
