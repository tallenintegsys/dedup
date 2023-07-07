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
#include <unistd.h>

auto operator<<(std::ostream &os, const Sha512 &rhs) -> std::ostream & {
	os << std::hex;
	for (unsigned char c : rhs) {
		os << std::setw(2) << std::setfill('0') << (int)c;
	}
	return os;
}

FileDB::FileDB() {
}

void FileDB::addFile(fs::directory_entry file) {

	Sha512 sha512 = calc_sha(file);
	filesBySHA.emplace(sha512, file);
	//	std::cout << filesBySHA.size() << ": " << std::string(file.path()) << ": " << sha512 << "\n";
}

void FileDB::printBySHA(void) {
	std::cout << "print By SHA\n";
	for (auto file : filesBySHA) {
		std::cout << file.first << "    " << file.second.path() << "\n";
	}
}

Sha512 FileDB::calc_sha(fs::directory_entry file) {
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
