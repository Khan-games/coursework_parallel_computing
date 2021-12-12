#pragma once

#include <iostream>
#include <string>

#include "cryptlib.h"
#include "rijndael.h"
#include "modes.h"
#include "files.h"
#include "osrng.h"
#include "hex.h"
#include "../index_creator/t_output.h"

namespace aes {	

	using namespace CryptoPP;

	class AESCustom
	{
	public:
		AESCustom();

		// main methods
		std::string encrypt(std::string plainMSG);
		std::string decrypt(std::string encryptedMSG);

		// getters
		SecByteBlock getKey();
		SecByteBlock getIV();

		// setters
		void setKey(SecByteBlock newKey);
		void setIV(SecByteBlock newIV);
		void setRandomAES();

		// conversion
		static std::string keyToString(SecByteBlock& key);
		static SecByteBlock stringToKey(std::string& str);

	private:
		AutoSeededRandomPool randomPool;

		SecByteBlock key;
		SecByteBlock iv;

	};

}



