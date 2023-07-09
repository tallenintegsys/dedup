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
	Sha512 sha = calcSha(dirent);
	ino_t inode = getInode(dirent);
	File file {dirent, inode, sha};
	filesBySha.emplace(sha, file);
	uniqueShas.emplace(sha);
	filesByInode.emplace(inode, file);
}

std::vector<FileDB2::File> FileDB2::filesWithSameSha(void) {
	std::vector<FileDB2::File> ret;
	
	for (auto fbs : filesBySha) {
		Sha512 sha = fbs.first;
		if (filesBySha.count(sha) > 1)
			ret.emplace_back(fbs.second);
	}
	return ret;
}

std::multimap<ino_t, FileDB2::File> FileDB2::filesWithSameSha(Sha512 sha) {
	std::multimap<ino_t, FileDB2::File> ret;
	auto range = filesBySha.equal_range(sha);
	for (auto p = range.first; p != range.second; p++) {
		ret.emplace(p->second.inode, p->second);
	}
	return ret;
}

void FileDB2::printFilesWithSameSha(void) {
	auto files = filesWithSameSha();
	std::cout << "printFilesWithSameSha\n";
	std::cout << "filename                      inode               sha\n";
	for (auto f : files) {
		std::cout << std::setfill(' ') << std::setw(30) << std::left << f.dirent;
		std::cout << std::setw(20) << f.inode;
		std::cout << std::setw(30) << f.sha << "\n";
	}
}

std::set<Sha512> FileDB2::findDupShas() {
	std::set<Sha512> dupShas; //shas that occur more than once but could be hard linked
	for (auto f : filesBySha) {
		Sha512 sha512 = f.first;
		if (filesBySha.count(sha512) < 2)
			continue; // there's only one
		dupShas.emplace(sha512);
	}
	return dupShas;
}

std::vector<FileDB2::File> FileDB2::filesWithSameShaDifferentInode(std::vector<FileDB2::File> files) {
	std::vector<FileDB2::File> ret;

	//std::multimap<ino_t, FileDB2::File> filesWithSameShaByInode = filesWithSameSha(sha);
	std::set<Sha512> dupShas = findDupShas();
	for (auto dupSha : dupShas) {

	}
	return ret;
}

void FileDB2::printFilesWithSameShaDifferentInode(void) {
	auto files = filesWithSameShaDifferentInode(filesWithSameSha());
	std::cout << "printFilesWithSameShaDifferentInode\n";
	std::cout << "filename                      inode               sha\n";
	for (auto f : files) {
		std::cout << f;
	}
}

void FileDB2::printDups(void) {
	std::cout << "print dups \n";
	for (auto sha : uniqueShas) {
		if (filesBySha.count(sha) == 1)
			continue; // skip if only one
		auto ssr = filesBySha.equal_range(sha);
		std::multimap<ino_t, FileDB2::File> filesWithSameSha;
		for (auto filePair = ssr.first; filePair != ssr.second; filePair++)
			filesWithSameSha.emplace(filePair->second.inode, filePair->second);
		ino_t inode = filesWithSameSha.begin()->first;
		for (auto fwss : filesWithSameSha) {
			if (fwss.second.inode != inode)
			std::cout << fwss.second;
		}
	}

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
}
