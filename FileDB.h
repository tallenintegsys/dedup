#include <filesystem>
#include <map>
#include <set>
#include <string>
#include <unistd.h>
#include <vector>

namespace fs = std::filesystem;

class Sha512 : public std::vector<unsigned char> {

	public:
	friend auto operator<<(std::ostream &, const Sha512 &) -> std::ostream &;
};

class FileDB {
	std::multimap<Sha512, fs::directory_entry> filesBySha;
	std::multimap<ino_t, fs::directory_entry> filesByInode;

	/// @brief
	/// @param
	/// @return
	Sha512 calcSha(const fs::directory_entry &);

	ino_t getInode(const fs::directory_entry &);

	bool isShaDup(const Sha512 &);

	bool isInodeDup(const ino_t &);

	bool isDup(const fs::directory_entry &, const Sha512 &, const ino_t &);

	public:
	//! Construct a ned FileDB
	/*!
		Contains multimaps of all files by inode and size
	*/
	FileDB();

	//! add File to containers
	/*!
		thus creating an ad-hoc DB
	*/
	void addFile(const fs::directory_entry &);

	//! Print all Files by inode
	/*!
		...for debuggering
	*/
	void printBySHA(void);

	/// @brief
	/// @param
	void printDups(void);

	//! cleanup your mess (RAII)
	/*!
		smart pointers would obviate this
	*/
	~FileDB();
};
