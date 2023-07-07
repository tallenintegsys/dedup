#include "FileDB.h"
#include <algorithm>
#include <dirent.h>
#include <fcntl.h>
#include <fstream>
#include <getopt.h>
#include <iomanip>
#include <iostream>
#include <list>
#include <openssl/evp.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>

auto operator<<(std::ostream &os, const Sha512 &rhs) -> std::ostream & {
	os << std::hex;
	for (unsigned char c : rhs) {
		os << std::setw(2) << std::setfill('0') << (int)c;
	}
	return os;
}

FileDB::FileDB() {
}

void FileDB::addFile(const fs::directory_entry &file) {

	Sha512 sha512 = calcSha(file);
	filesBySHA.emplace(sha512, file);

	ino_t inode = getInode(file);
	filesByInode.emplace(inode, file);
	/*
		bool d = isDup(inode, file);

		if (shaIsDup(sha512)) {
			if ()
			std::cout << file.path() << "   " << file.hard_link_count() << "  possible dup ";
			if ((file.hard_link_count() == 1) && !file.is_symlink()) {
				dupSHAs.insert(sha512);
				std::cout << " is a dup";
			} else {
				std::cout << " is a link";
			}
			std::cout << std::endl;
		}
		*/
	//	std::cout << filesBySHA.size() << ": " << std::string(file.path()) << ": " << sha512 << "\n";
}

void FileDB::printBySHA(void) {
	std::cout << "print By SHA\n";
	for (auto file : filesBySHA) {
		std::cout << file.first << "    " << file.second.hard_link_count() << "  " << file.second.path() << "\n";
	}
}

void FileDB::printDups(void) {
	std::cout << "print dups \n";
	/*for (auto dup : dupSHAs) {
		auto range = filesBySHA.equal_range(dup);
		for (auto i = range.first; i != range.second; ++i) {
			std::cout << i->second.path() << "\n";
		}
	}
	*/
}

bool FileDB::isShaDup(const Sha512 &sha512) {
	return filesBySHA.count(sha512) > 1;
}

bool FileDB::isInodeDup(const ino_t &inode) {
	return filesByInode.count(inode) > 1;
}

bool FileDB::isDup(const fs::directory_entry &file, const Sha512 &sha512, const ino_t &inode) {
	if (!isShaDup(sha512))
		return false;

	std::vector<fs::directory_entry> files;
	auto fs = filesBySHA.equal_range(sha512);
	for (auto i = fs.first; i != fs.second; ++i) {
		ino_t inode = getInode(i->second);
		auto fi = filesByInode.equal_range(inode);
		for (auto j = fi.first; j != fi.second; ++j) {
			files.emplace_back(j->second);
		}
	}
	return true;
}
ino_t FileDB::getInode(const fs::directory_entry &file) {
	struct stat buf;
	int rs = stat(file.path().c_str(), &buf);
	if (!rs)
		exit(EXIT_FAILURE); // XXX do better
	return buf.st_ino;
}

Sha512 FileDB::calcSha(const fs::directory_entry &file) {
	EVP_MD_CTX *mdctx;
	const EVP_MD *md;
	char *file_buffer;
	unsigned int md_len;
	unsigned char md_value[EVP_MAX_MD_SIZE];

	OpenSSL_add_all_digests();

	md = EVP_get_digestbyname("sha512");

	if (!md) {
		printf("Unknown message digest \n");
		exit(EXIT_FAILURE); // XXX handle this better?
	}

	int file_descript = open(file.path().c_str(), O_RDONLY);
	if (file_descript < 0) {
		perror("open for sha");
		exit(EXIT_FAILURE);
	}

	file_buffer = (char *)mmap(NULL, file.file_size(), PROT_READ, MAP_SHARED, file_descript, 0);
	if (file_buffer == MAP_FAILED) {
		perror("mmap");
		exit(EXIT_FAILURE);
	}

	mdctx = EVP_MD_CTX_create();
	EVP_DigestInit_ex(mdctx, md, NULL);
	EVP_DigestUpdate(mdctx, file_buffer, strlen(file_buffer));
	EVP_DigestFinal_ex(mdctx, md_value, &md_len);
	EVP_MD_CTX_destroy(mdctx);

	/* Call this once before exit. */
	EVP_cleanup();
	munmap(file_buffer, strlen(file_buffer));
	close(file_descript);
	Sha512 res;
	for (int i = 0; i < 64; i++)
		res.emplace_back(md_value[i]);
	return res;
};

FileDB::~FileDB() {
}
