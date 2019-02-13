#include "File.h"

File::File(const std::string &name) {
	this->name = name;
	this->relname = name.substr(name.find_first_of("/") + 1);
	this->subname = relname.substr(relname.find_first_of("/") + 1);
	struct stat sb;
	if (stat(name.c_str(), &sb) == -1) {
		std::cout << name << std::endl;
		perror("stat");
		exit(EXIT_FAILURE);
	}
	inode = sb.st_ino;
	size = sb.st_size;
	hardlinks = sb.st_nlink;
}

void File::link(File *file) {
	dup = true;
	file->dup = true; //	XXX this is for testing
}

auto File::operator==(File &rhs) -> int {
	if ((size == 0) || (rhs.size == 0)) {
		return equality::no; //	ignore empty files
	}
	File *foundfile;
	//	check inode first, it's computationally cheaper
	if (rhs.inode == this->inode)
		return equality::hardlink;
	//  check size
	if (size == rhs.size) {
		if (sha == NULL) {
			calc_sha();
		}
		if (rhs.sha == NULL) {
			rhs.calc_sha();
		}
		return memcmp(sha, rhs.sha, EVP_MAX_MD_SIZE) ? equality::no : equality::yes;
	}
	return equality::no; //	different sizes
}

void File::calc_sha() {
	EVP_MD_CTX *mdctx;
	const EVP_MD *md;
	char *file_buffer;
	unsigned int md_len;
	unsigned char *md_value = new unsigned char[EVP_MAX_MD_SIZE];

	if (size == 0) { //	this can happen
		delete[] md_value;
		return;
	}

	OpenSSL_add_all_digests();

	md = EVP_get_digestbyname("sha512");

	if (!md) {
		printf("Unknown message digest \n");
		exit(EXIT_FAILURE);
	}

	int file_descript = open(name.c_str(), O_RDONLY);
	if (file_descript < 0) {
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

auto File::isHardlink(File *file) -> bool {
	return file->inode == this->inode;
}

auto operator<<(std::ostream &os, const File &rhs) -> std::ostream & {
	/* show detailed debug info
os << rhs.inode;
os << "\e[" << 15 - std::to_string(rhs.inode).size() << "C" << rhs.size;
os << "\e[" << 10 - std::to_string(rhs.size).size() << "C" << rhs.hardlinks;
os << "\e[" << 6 - std::to_string(rhs.hardlinks).size() << "C" << rhs.relname;
os << "\e[" << 39 - rhs.relname.size() << "C" << rhs.name;
os << "\e[" << 39 - rhs.name.size() << "C"; */
	os << rhs.name;
	return os;
}
