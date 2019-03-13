#include "encryption.h"
#if defined(__client_build)
Cipher::Cipher(int n)
	: map{ make_map(n) } {
};
std::unordered_map<char, char> Cipher::make_map(int n) {
	
};

std::string Cipher::encrypt(std::string s) const {
	
}
Cipher::~Cipher() {
	
}
;
#endif//!__client_build