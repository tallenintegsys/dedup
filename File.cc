#include "File.h"

std::map<__ino_t, File *> File::uk_inode;
std::multimap<fsize_t, File *> File::cx_size;

File::File(const std::string &path, const std::string &filename) {
	name = filename;
	relativepath.append(path) += std::string("/") += filename;
	std::cout << relativepath << std::endl;
	struct stat sb;

	if (stat(relativepath.c_str(), &sb) == -1) {
		perror("stat");
		exit(EXIT_FAILURE);
	}
	inode = sb.st_ino;
	size = sb.st_size;

	auto r = uk_inode.insert(std::pair<__ino_t, File*>(inode, this));
	if (!r.second) { //duplicate inode, must already be a hardlink
		//std::cout << filename << " dup" << std::endl;
		hardlink = true;
		r.first->second->hardlink = true;
	}
	cx_size.insert(std::pair<fsize_t, File *>(size, this));
	if (cx_size.count(size) > 1) {
		//std::multimap<fsize_t, File*>::iterator it = cx_size.find(size);
		auto rp = cx_size.equal_range(size);
		for(auto it = rp.first; it != rp.second; it++) {
			if (this == it->second)
				continue;
			if (equal(*it->second))// find the other identically sized file(s)
				link(it->second);
		}
	}
}

void File::link(File* file) {
	std::cout << "link0: " << name << "   " << inode << "   ";
	std::cout << "link1: " << file->name << "   " << file->inode << "   ";
	std::cout << std::endl << std::endl;
}

bool File::equal(File &rhs) {
	if ((size == 0) || (rhs.size == 0))
		return false; //ignore empty files

	if (size == rhs.size) {
		if (sha	== NULL)
			calc_sha();
		if (rhs.sha == NULL)
			rhs.calc_sha();
		return sha == rhs.sha;
	}
	return false;
}

unsigned char *File::calc_sha() {
	EVP_MD_CTX *mdctx;
	const EVP_MD *md;
	char *file_buffer;
	unsigned int md_len;
	unsigned char *md_value = new unsigned char [EVP_MAX_MD_SIZE+1];

	if (size == 0) //this can happen
		return NULL;

	OpenSSL_add_all_digests();

	md = EVP_get_digestbyname("sha512");

	if(!md) {
		printf("Unknown message digest \n");
		exit(EXIT_FAILURE);
	}

	int file_descript = open(relativepath.c_str(), O_RDONLY);
	if(file_descript < 0) exit(-1);

	file_buffer = (char *)mmap(NULL, size, PROT_READ, MAP_SHARED, file_descript, 0);

	mdctx = EVP_MD_CTX_create();
	EVP_DigestInit_ex(mdctx, md, NULL);
	EVP_DigestUpdate(mdctx, file_buffer, strlen(file_buffer));
	EVP_DigestFinal_ex(mdctx, md_value, &md_len);
	EVP_MD_CTX_destroy(mdctx);

	/* Call this once before exit. */
	EVP_cleanup();
	return md_value;
};

bool File::isHardlink(File *file) {
	return file->inode == this->inode;
}
