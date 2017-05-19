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

	auto r = uk_inode.insert(std::pair<__ino_t, File *>(inode, this));
	if (!r.second) { //duplicate inode, must already be a hardlink
		std::cout << filename << " dup" << std::endl;
		hardlink = true;
		r.first->second->hardlink = true;
	}
	cx_size.insert(std::pair<fsize_t, File *>(size, this));
	if (cx_size.count(size) > 1) {
		// calc the md5 for this entrant
		sha = calc_sha();
	}
	if (cx_size.count(size) == 2) {
		// find the other identically sized file and calc it's sha512
		// acording to the spec the first one is garenteed to be first in the container
		std::multimap<fsize_t, File *>::iterator it = cx_size.find(size);
		it->second->calc_sha();
	}
}

unsigned char *File::calc_sha() {
	EVP_MD_CTX *mdctx;
	const EVP_MD *md;
	char *file_buffer;
	unsigned int md_len, i;
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

	std::cout << md_len << std::endl;
	printf("Digest is: ");
	for(i = 0; i < md_len; i++)
		printf("%02x", md_value[i]);
	printf("\n");

	/* Call this once before exit. */
	EVP_cleanup();
	return md_value;
};

bool File::isHardlink(File *file) {
	return file->inode == this->inode;
}
