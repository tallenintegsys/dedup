#ifndef _SHA512_H
#define _SHA512_H 1

#include <iomanip>
#include <iostream>
#include <vector>

class Sha512 : public std::vector<unsigned char> {

	public:
	friend auto operator<<(std::ostream &os, const Sha512 &rhs) -> std::ostream & {
		os << std::hex;
		for (int i = 0; i < 8; i++)
			os << std::setw(2) << std::setfill('0') << (int)rhs[i];

		os << std::dec;
		return os;
	}
};
#endif //_SHA512_H