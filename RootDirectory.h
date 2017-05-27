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
#include "File.h"

class RootDirectory {

	std::multimap<__ino_t, File *> filesbyinode;
	std::multimap<fsize_t, File *> filesbysize;

	AddFile(File &file);

	public:
	RootDirectory(std::string& path);
	~RootDirectory();
};
