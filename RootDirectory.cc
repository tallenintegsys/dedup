#include "RootDirectory.h"

RootDirectory::RootDirectory(std::string& path) {
	struct dirent **de;

	int n = scandirat(AT_FDCWD, path.c_str(), &de, NULL, alphasort);
	if (n == -1)
		return;

	while(n--) {
		if (!strcmp(de[n]->d_name, "..") || !strcmp(de[n]->d_name, "."))
			continue;

		if (de[n]->d_type == DT_DIR) {
			//continue; //XXX just testing for now don't descend the tree
			//directory
			std::string p(path);
			p+=std::string("/")+=std::string(de[n]->d_name);
			RootDirectory(p); //recurse
		}
		if (de[n]->d_type == DT_REG) {
			//regular file
			std::string filename(de[n]->d_name);
			//std::cout << filename << std::endl;
			File *file = new File(path, filename); //this is actually not a memory leak
			AddFile(file);

		}
	}
	free(de);
}

RootDirectory::AddFile(File &file) {

	// insert into the inode table
	uk_inode.insert(std::pair<__ino_t, File*>(inode, this));
	if (sb.st_nlink > 1)
		hardlink = true;

	// insert into the size table
	cx_size.insert(std::pair<fsize_t, File *>(size, this));
	if ((cx_size.count(size) > 1) && (!hardlink)) {
		auto rp = cx_size.equal_range(size); //range of file with same size
		for(auto it = rp.first; it != rp.second; it++) {
			if (this == it->second)
				continue; //well, we don't want to link to ourselves
			if (equal(*it->second))// find the other identically sized file(s)
				link(it->second); //make a hardlink
		}
	}
}

RootDirectory::~RootDirectory() {

}
