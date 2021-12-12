#pragma once

#include <boost/multiprecision/miller_rabin.hpp>
#include <boost/random/mersenne_twister.hpp>

#include "../index_creator/t_output.h"

#define RSA_KEY_SIZE 256; // how much bits RSA key would have
#define PUBLIC_KEY_EXP 65537; // popular choice for the public exponent

namespace rsa {

	using namespace boost::multiprecision;
	using namespace boost::random;

	// struct to store RSA keys
	typedef struct Key {
		cpp_int exp;
		cpp_int modulus;
	} Key;

	class RSACustom
	{
	public:
		RSACustom(); // constructor initiates generation of keypair

		// main methods
		cpp_int encrypt(std::string plainMSG);  // to encrypt, set up the remote key at first
		cpp_int encrypt(cpp_int plainMSG); // to encrypt, set up the remote key at first
		cpp_int encryptSign(std::string plainMSG);
		cpp_int encryptSign(cpp_int plainMSG);
		std::string decrypt(cpp_int encryptedMSG);
		std::string decryptSign(cpp_int encryptedMSG); // to decrypt, set up the remote key at first
		std::string decryptAndUnsign(cpp_int encryptedMSG); // to decrypt, set up the remote key at first

		// key generation
		void generateKeyPair();

		// setters
		void setRemotePublicKey(Key newRemotePublicKey);

		// getters
		Key getPublicKey();

		// static functions
		static cpp_int getPrime(); // function to find 512 bit prime number
		static std::string toString(cpp_int cppInt);
		static cpp_int toCppInt(std::string text);

	private:
		// keys
		Key privateKey;
		Key publicKey;
		Key remotePublicKey = { 0, 0 }; // empty remote key on start

		// internal functions 
		cpp_int extendedEuclid(cpp_int e, cpp_int fi, cpp_int& d, cpp_int& y);
		cpp_int fastExp(cpp_int x, cpp_int y, cpp_int N); // x^y mod N
	};



}



