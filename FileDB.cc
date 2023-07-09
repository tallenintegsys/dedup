#include "FileDB.h"
#include <algorithm>
#include <dirent.h>
#include <fcntl.h>
#include <fstream>
#include <getopt.h>
#include <iostream>
#include <list>
#include <openssl/evp.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>

FileDB::FileDB(bool relink) {
	FileDB::relink = relink;
}

void FileDB::addFile(const fs::directory_entry &dirent) {
	Sha512 sha = calcSha(dirent);
	ino_t inode = getInode(dirent);
	File file{dirent, inode, sha};
	filesBySha.emplace(sha, file);
	uniqueShas.emplace(sha);
}

void FileDB::printDups(void) {
	std::cout << "print dups \n";
	for (auto sha : uniqueShas) {
		if (filesBySha.count(sha) == 1)
			continue; // skip if only one
		auto ssr = filesBySha.equal_range(sha);
		std::multimap<ino_t, const FileDB::File> filesWithSameSha;
		for (auto filePair = ssr.first; filePair != ssr.second; filePair++)
			filesWithSameSha.emplace(filePair->second.inode, filePair->second);
		File firstFile = filesWithSameSha.begin()->second;
		for (auto fwss : filesWithSameSha) {
			if (fwss.second.inode != firstFile.inode) {
				std::cout << fwss.second;
				if (relink) {
					std::cout << "rm " << fwss.second.dirent.path() << "\n";
					std::cout << "ln " << firstFile.dirent.path() << " " << fwss.second.dirent.path() << "\n";
				}
			}
		}
	}
}

ino_t FileDB::getInode(const fs::directory_entry &file) {
	struct stat buf;
	int rs = stat(file.path().c_str(), &buf);
	if (rs != 0) {
		std::perror("FileDB2::getInode");
		exit(EXIT_FAILURE);
	}
	return buf.st_ino;
}

Sha512 FileDB::calcSha(const fs::directory_entry &dirent) {
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

	int file_descript = open(dirent.path().c_str(), O_RDONLY);
	if (file_descript < 0) {
		perror("open for sha");
		exit(EXIT_FAILURE);
	}

	file_buffer = (char *)mmap(NULL, dirent.file_size(), PROT_READ, MAP_SHARED, file_descript, 0);
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
}

FileDB::~FileDB() {
}
