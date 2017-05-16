#include <dirent.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <string>
#include <boost/multiprecision/cpp_int.hpp>
#include <map>
#include <sys/mman.h>
#include <openssl/evp.h>





typedef off_t fsize_t;

class File {
	public:
		__ino_t inode;
		std::string name;
		std::string relativepath;
		fsize_t size;
		unsigned char *sha = 0;
		static std::map<ino_t, File *> uk_inode;
		static std::multimap<fsize_t, ino_t> cx_size;

		File(const std::string &path, const std::string &filename) {
			name = filename;
			relativepath.append(path) += std::string("/") += filename;
			struct stat sb;

			if (stat(relativepath.c_str(), &sb) == -1) {
				perror("stat");
				exit(EXIT_FAILURE);
			}
			inode = sb.st_ino;
			size = sb.st_size;

			uk_inode.insert(std::pair<ino_t, File *>(inode, this));
			cx_size.insert(std::pair<fsize_t, ino_t>(size, inode));
			if (cx_size.count(sb.st_size) > 1) {
				// calc the md5 for this entrant
				sha = calc_sha();
			}
			if (cx_size.count(sb.st_size) == 2) {
				// find the other identically sized file and calc it's sha512
			}
			// the others files should have already had there sha512s calc'd
		}
	private:
		unsigned char *calc_sha() {
			EVP_MD_CTX *mdctx;
			const EVP_MD *md;
			char *file_buffer;
			unsigned int md_len, i;
			unsigned char *md_value = new unsigned char [EVP_MAX_MD_SIZE+1];

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
};


std::map<ino_t, File *> File::uk_inode;
std::multimap<fsize_t, ino_t> File::cx_size;

void statdir(const std::string& path) {
	struct dirent **de;

	int n = scandirat(AT_FDCWD,path.c_str(), &de, NULL, alphasort); 
	if (n == -1)
		return;

	while(n--) {
		if (!strcmp(de[n]->d_name, "..") || !strcmp(de[n]->d_name, "."))
			continue;
		
		if (de[n]->d_type == DT_DIR) {
			//directory
			std::string p(path);
			p+=std::string("/")+=std::string(de[n]->d_name);
			statdir(p);
		}
		if (de[n]->d_type == DT_REG) {
			//regular file
			std::string filename(de[n]->d_name);
			new File(path, filename);
		}
	}
}

int main(int argv, char **argc) {

	if (argv != 2) {
		perror("Insuficient arguments");
		exit(EXIT_FAILURE);
	}
	statdir(argc[1]);

	std::cout << "By inode:"<< std::endl;
	for (auto f : File::uk_inode) {
		File *ff = f.second;
		if (ff->sha) {
		std::cout<<ff->inode<<"\t "<<ff->size<<"\t "<<"\t "<<ff->name <<"\t ";
		if (ff->sha) {
			for(int i = 0; i < 64 ; i++)
				printf("%02x", ff->sha[i]);
			printf("\n");

		} else {
			std::cout << std::endl;
		}
	}
	}

	std::cout << std::endl << "By size:"<< std::endl;
	for (auto f : File::cx_size) {
		__ino_t i = f.second;
		File *ff = File::uk_inode[i];
		std::cout<<ff->inode<<"\t "<<ff->size<<"\t "<<"\t "<<ff->name<<"\t "<<ff->sha << std::endl;
	}

	exit(EXIT_SUCCESS);
}

