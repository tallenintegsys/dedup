#include <filesystem>
#include <map>
#include <string>
#include <unistd.h>
#include <vector>

namespace fs = std::filesystem;

class Sha512 : public std::vector<unsigned char> {

	public:
	friend auto operator<<(std::ostream &, const Sha512 &) -> std::ostream &;
};

class FileDB {
	std::multimap<Sha512, fs::directory_entry> filesBySHA;

	/// @brief
	/// @param
	/// @return
	Sha512 calc_sha(fs::directory_entry);

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
	void addFile(fs::directory_entry);

	//! Print all Files by inode
	/*!
		...for debuggering
	*/
	void printBySHA(void);

	//! cleanup your mess (RAII)
	/*!
		smart pointers would obviate this
	*/
	~FileDB();
};
