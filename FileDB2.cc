#include "FileDB2.h"
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


FileDB2::FileDB2() {
}

void FileDB2::addFile(const fs::directory_entry &dirent) {
	Sha512 sha512 = calcSha(dirent);
	ino_t inode = getInode(dirent);
	File file {dirent, inode, sha512};
	auto fbs = filesBySha.emplace(sha512, file);
	auto fbi = filesByInode.emplace(inode, file);
}

void FileDB2::printDups(void) {

	std::cout << "print dups \n";

	// find possible dups
	std::multimap<Sha512, fs::directory_entry> filesWithSameSha;
	std::set<Sha512> possibleDups; //shas occur more than once but could be hard linked
	for (auto f : filesBySha) {
		Sha512 sha512 = f.first;
		if (filesBySha.count(sha512) < 2)
			continue; // there's only one
		if (filesWithSameSha.count(sha512) > 0)
			continue; // already got this one
		auto range = filesBySha.equal_range(sha512);
		for (auto r = range.first; r != range.second; ++r)
			filesWithSameSha.emplace(r->first, r->second.dirent);
		possibleDups.emplace(sha512);
	}

	// find actual dups (not hard links)
	for (Sha512 sha512 : possibleDups) {
		std::cout << "checking " << sha512 << "...\n";
		std::multimap<ino_t, fs::directory_entry> filesWithSameShaByInode;
		auto range = filesWithSameSha.equal_range(sha512);
		for (auto fileWithSameSha = range.first; fileWithSameSha != range.second; ++fileWithSameSha) {
			std::cout << fileWithSameSha->first << "   " << fileWithSameSha->second << "\n";
			ino_t inode = getInode(fileWithSameSha->second);
			filesWithSameShaByInode.emplace(inode, fileWithSameSha->second);
		}
		ino_t inode = filesWithSameShaByInode.begin()->first;
		for (auto f : filesWithSameShaByInode) {
			std::cout << std::dec << f.first << "   " << f.second;
			if (f.first == inode)
				std::cout << " is already hard linked\n";
			else
				std::cout << " is a duplicate\n";
		}
		std::cout << "select * from filesWithSameShaByInode where Inode unique\n";
		for(auto it = filesWithSameShaByInode.begin(), end = filesWithSameShaByInode.end(); it != end; it = filesWithSameShaByInode.upper_bound(it->first)){
			std::cout << it->first << ' ' << it->second << "\n";
	}
		/*if (filesWithSameShaByInode.count(sha512) > 1)
			std::cout << " is a dup\n";
		else
			std::cout << " is not a dup\n";
*/
		filesWithSameShaByInode.clear();
	};

}

bool FileDB2::isShaDup(const Sha512 &sha512) {
	return filesBySha.count(sha512) > 1;
}

bool FileDB2::isInodeDup(const ino_t &inode) {
	return filesByInode.count(inode) > 1;
}

ino_t FileDB2::getInode(const fs::directory_entry &file) {
	struct stat buf;
	int rs = stat(file.path().c_str(), &buf);
	if (rs != 0) {
		std::perror("FileDB2::getInode");
		exit(EXIT_FAILURE);
	}
	return buf.st_ino;
}

Sha512 FileDB2::calcSha(const fs::directory_entry &dirent) {
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

FileDB2::~FileDB2() {
	filesByInode.clear();
	filesBySha.clear();
	std::cout << "FileDB2 destroyed" << std::endl;
}
