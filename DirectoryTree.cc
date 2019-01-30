#include "DirectoryTree.h"
#include <dirent.h>
#include <iomanip>
#include <iostream>
#include <string.h>
#include <unistd.h>

std::vector<DirectoryTree *> DirectoryTree::trees = std::vector<DirectoryTree *>();

DirectoryTree::DirectoryTree(const std::string &root) {
	trees.push_back(this);
	this->id = trees.size();//e.g. 1, 2, 3..n
	this->root = (root[root.size() - 1] == '/') ? root.substr(0, root.size() - 1) : root;
	scan("");
}

void DirectoryTree::scan(std::string relpath) {
	struct dirent **de;
	std::string fullpath(root);
	fullpath += "/";
	if (relpath.size() > 0) {
		fullpath += relpath;
		fullpath += "/";
	}
	int n = scandirat(AT_FDCWD, fullpath.c_str(), &de, NULL, alphasort);
	if (n < 0) {
		std::cout << fullpath << "  ";
		perror("scandirat");
		return;
	}
	while (n--) {
		if (!strcmp(de[n]->d_name, "..") || !strcmp(de[n]->d_name, "."))
			continue;

		if (de[n]->d_type == DT_DIR) {
			//	directory
			scan(de[n]->d_name); //	recurse
		}
		if (de[n]->d_type == DT_REG) {
			//	regular file
			std::string filename(de[n]->d_name);
			fullpath += de[n]->d_name;
			File *file = new File(root, relpath, filename);
			AddFile(file);
		}
	}
	free(de);
}

void DirectoryTree::AddFile(File *file) {

	// insert into the inode table
	filesbyinode.insert(std::pair<__ino_t, File *>(file->inode, file));

	// insert into the size table
	filesbysize.insert(std::pair<size_t, File *>(file->size, file));

	// insert into relativepath table
	std::string rp;
	if (file->relpath.size() > 0) {
		rp += file->relpath;
		rp += "/";
	}
	rp += file->name;
	filesbyrelativepath.insert(std::pair<std::string, File *>(rp, file));

	// find identical files (candidates for hard linking)
	for (DirectoryTree *rd : trees) {
		if (rd == this)
			continue; //	skip ourself
		std::string relname(file->relpath);
		if (relname.size() != 0)
			relname += "/";
		relname += file->name;
		if (rd->filesbyrelativepath.count(relname)) { //	same name?
			File *dfile = rd->filesbyrelativepath[relname];
			if ((*file) == (*dfile)) {
				std::cout << file->fullpath;
				std::cout << " = ";
				std::cout << dfile->fullpath;
				std::cout << std::endl;
			}
		}
	}
}

void DirectoryTree::PrintByInode(void) {
	std::cout << "By inode:" << std::left << std::endl;
	for (auto pf : filesbyinode) {
		File *f = pf.second;
		if (f->hardlink)
			std::cout << "H";
		if (f->dup)
			std::cout << "D";

		std::cout << " \t";
		std::cout << std::setw(10) << f->inode << std::setw(10) << f->size << std::setw(40) << f->name.substr(0, 35) << "\t";
		if (f->sha) {
			for (int i = 0; i < 64; i++) {
				if (i == 10) {
					std::cout << "...";
					i += 44;
				}
				printf("%02x", f->sha[i]);
			}
			printf("\n");
		} else {
			std::cout << std::endl;
		}
	}
	std::cout << std::right << std::endl;
}

void DirectoryTree::PrintBySize(void) {
	std::cout << std::endl << "By size:" << std::left << std::endl;
	for (auto pf : filesbysize) {
		auto f = pf.second;
		std::cout << std::setw(10) << f->inode << std::setw(10) << f->size << std::setw(40) << f->name.substr(0, 35) << "\t";
		if (f->sha) {
			for (int i = 0; i < 64; i++) {
				if (i == 10) {
					std::cout << "...";
					i += 44;
				}
				printf("%02x", f->sha[i]);
			}
			printf("\n");

		} else {
			std::cout << std::endl;
		}
	}
	std::cout << std::right << std::endl;
}

void DirectoryTree::PrintByRelativepath(void) {
	std::cout << std::endl << "By relative path:" << std::left << std::endl;
	for (auto pf : filesbyrelativepath) {
		auto f = pf.second;
		std::cout << std::setw(30) << pf.first << std::setw(10) << f->size << std::setw(20) << f->name.substr(0, 35) << "\t";
		if (f->sha) {
			for (int i = 61; i < 64; i++) {
				printf("%02x", f->sha[i]);
			}
			printf("\n");
		} else {
			std::cout << std::endl;
		}
	}
	std::cout << std::right << std::endl;
}

DirectoryTree::~DirectoryTree() {
	for (auto it = filesbysize.begin(); it != filesbysize.end(); it++)
		delete (*it).second;
}