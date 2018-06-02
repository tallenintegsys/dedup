#include "File.h"

File::File(const std::string &root, const std::string &relpath, const std::string &name) {
	this->root = root;
	this->relpath = relpath;
	this->name = name;
	this->fullpath += root;
	if (relpath.size() > 0)
		this->fullpath += std::string("/") += relpath;
	this->fullpath += std::string("/") += name;
	struct stat sb;
	if (stat(fullpath.c_str(), &sb) == -1) {
		perror("stat");
		exit(EXIT_FAILURE);
	}
	inode = sb.st_ino;
	size = sb.st_size;
	nlink = sb.st_nlink;
}

void File::link(File* file) {
	dup = true;
	file->dup = true;//XXX this is for testing
}

bool File::operator==(File &rhs) {
	if ((size == 0) || (rhs.size == 0))
		return false; //ignore empty files

	if (size == rhs.size) {
		if (sha	== NULL)
			calc_sha();
		if (rhs.sha == NULL)
			rhs.calc_sha();
		return memcmp(sha, rhs.sha, EVP_MAX_MD_SIZE) == 0;
	}
	return false;
}

void File::calc_sha() {
	EVP_MD_CTX *mdctx;
	const EVP_MD *md;
	char *file_buffer;
	unsigned int md_len;
	unsigned char *md_value = new unsigned char [EVP_MAX_MD_SIZE];

	if (size == 0) { //this can happen
		delete[] md_value;
		return;
	}

	OpenSSL_add_all_digests();

	md = EVP_get_digestbyname("sha512");

	if(!md) {
		printf("Unknown message digest \n");
		exit(EXIT_FAILURE);
	}

	int file_descript = open(fullpath.c_str(), O_RDONLY);
	if(file_descript < 0) {
		perror("open for sha");
		exit(EXIT_FAILURE);
	}

	file_buffer = (char *)mmap(NULL, size, PROT_READ, MAP_SHARED, file_descript, 0);
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
	sha = md_value;
	return;
};

bool File::isHardlink(File *file) {
	return file->inode == this->inode;
}
