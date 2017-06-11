#include "RootDirectory.h"

std::vector<RootDirectory*> RootDirectory::rootdirectories = std::vector<RootDirectory*>();

RootDirectory::RootDirectory(const std::string &root) {
	rootdirectories.push_back(this);
	this->id = rootdirectories.size();
	this->root = (root[root.size()-1] == '/') ? root.substr(0,root.size()-1) : root;
	scan("");
}


void RootDirectory::scan(std::string relpath) {
	struct dirent **de;
	std::string fullpath(root);
	fullpath += "/";
	if (relpath.size() > 0) {
		fullpath += relpath;
		fullpath += "/";
	}
	int n = scandirat(AT_FDCWD, fullpath.c_str(), &de, NULL, alphasort);
	if (n == -1)
		return;

	while(n--) {
		if (!strcmp(de[n]->d_name, "..") || !strcmp(de[n]->d_name, "."))
			continue;

		if (de[n]->d_type == DT_DIR) {
			//directory
			scan(de[n]->d_name); //recurse
		}
		if (de[n]->d_type == DT_REG) {
			//regular file
			std::string filename(de[n]->d_name);
			fullpath += de[n]->d_name;
			File *file = new File(root, relpath, filename);
			AddFile(file);
		}
	}
	free(de);
}


void RootDirectory::AddFile(File *file) {

	// insert into the inode table
	filesbyinode.insert(std::pair(file->inode, file));

	// insert into the size table
	filesbysize.insert(std::pair(file->size, file));

	// insert into relativepath table
	std::string rp;
	if (file->relpath.size() > 0) {
		rp += file->relpath;
		rp += "/";
	}
	rp += file->name;
	filesbyrelativepath.insert(std::pair(rp, file));

	// find identical files (candidates for hard linking)
	/*
	if ((filesbysize.count(file->size) > 1) && (!file->hardlink)) {
		auto rp = filesbysize.equal_range(file->size); //range of files with same size
		for(auto it = rp.first; it != rp.second; it++) {
			if (file == it->second)
				continue; //well, we don't want to link to ourselves
			if (file->equal(*it->second))// find the other identically sized file(s)
				file->link(it->second); //make a hardlink
		}
	}
	*/
}


void RootDirectory::PrintByInode (void) {
	std::cout << "By inode:"<< std::left << std::endl;
	for (auto pf : filesbyinode) {
		File *f = pf.second;
		if (f->hardlink)
			std::cout << "H";
		if (f->dup)
			std::cout << "D";

		std::cout << " \t";
		std::cout<<std::setw(10) << f->inode << std::setw(10) << f->size << std::setw(40) << f->name.substr(0,35) << "\t" ;
		if (f->sha) {
			for(int i = 0; i < 64 ; i++) {
				if (i==10) {
					std::cout << "...";
					i+=44;
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


void RootDirectory::PrintBySize (void) {
	std::cout << std::endl << "By size:"<< std::left << std::endl;
	for (auto pf : filesbysize) {
		auto f = pf.second;
		std::cout<<std::setw(10) << f->inode << std::setw(10) << f->size << std::setw(40) << f->name.substr(0,35) << "\t" ;
		if (f->sha) {
			for(int i = 0; i < 64 ; i++) {
				if (i==10) {
					std::cout << "...";
					i+=44;
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


void RootDirectory::PrintByRelativepath (void) {
	std::cout << std::endl << "By relative path:"<< std::left << std::endl;
	for (auto pf : filesbyrelativepath) {
		auto f = pf.second;
		std::cout<<std::setw(30) << pf.first << std::setw(10) << f->size << std::setw(40) << f->name.substr(0,35) << "\t" ;
		if (f->sha) {
			for(int i = 0; i < 64 ; i++) {
				if (i==10) {
					std::cout << "...";
					i+=44;
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


RootDirectory::~RootDirectory() {
	for (auto it = filesbysize.begin(); it != filesbysize.end(); it ++)
		delete (*it).second;
}
