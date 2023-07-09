#ifndef _SHA512_H
#define _SHA512_H 1

#include <iomanip>
#include <iostream>
#include <vector>

class Sha512 : public std::vector<unsigned char> {

	public:
	friend auto operator<<(std::ostream &os, const Sha512 &rhs) -> std::ostream & {
		os << std::hex << std::setfill('0') << std::right;
		for (int i = 0; i < 8; i++)
			os << std::setw(2) << (int)rhs[i];

		os << std::dec << std::setfill(' ') << std::left;
		return os;
	}
};
#endif //_SHA512_H