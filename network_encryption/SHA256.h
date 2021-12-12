#pragma once

#include <iostream>
#include <string>
#include <openssl/sha.h>
#include <memory>

class SHA256Custom
{
public:
	SHA256Custom(std::string msg);
	std::string hashOf(std::string msg);
	
	// result actions
	std::string getHash();
	void printAsHex();
	void printAsChar();

	static bool isEqual(SHA256Custom hash1, SHA256Custom hash2);
	static bool isEqual(std::string hash1, std::string hash2);

private:
	std::string hash;
};

