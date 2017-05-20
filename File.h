#include <dirent.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string>
#include <boost/multiprecision/cpp_int.hpp>
#include <map>
#include <sys/mman.h>
#include <openssl/evp.h>

typedef off_t fsize_t;

class File {
	public:
		__ino_t inode;
		std::string name;
		std::string relativepath;
		fsize_t size;
		unsigned char *sha = NULL;
		bool hardlink;
		static std::map<__ino_t, File *> uk_inode;
		static std::multimap<fsize_t, File *> cx_size;

		File(const std::string &, const std::string &);
		bool operator== (File &);
	private:
		unsigned char *calc_sha();
		bool isHardlink(File *);
};
