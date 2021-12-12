#include "AESCustom.h"

namespace aes {



	AESCustom::AESCustom() :
        key(AES::DEFAULT_KEYLENGTH),
        iv(AES::BLOCKSIZE)
    {
        // generate key and iv
        randomPool.GenerateBlock(key, key.size());
        randomPool.GenerateBlock(iv, iv.size());
	}

    std::string AESCustom::encrypt(std::string plainMSG)
    {
        std::string encodedMsg;
        try
        {
            // set up
            CBC_Mode< AES >::Encryption e;
            e.SetKeyWithIV(key, key.size(), iv);

            // encoding
            StringSource s(plainMSG, true,
                    new StreamTransformationFilter(e, new StringSink(encodedMsg))); 

            return encodedMsg;
        }
        catch (const Exception& e)
        {
            std::cerr << e.what() << std::endl;
            exit(1);
        }
    }

    std::string AESCustom::decrypt(std::string encryptedMSG)
    {
        std::string plainMSG;
        try
        {
            // set up
            CBC_Mode< AES >::Decryption d;
            d.SetKeyWithIV(key, key.size(), iv);

            // decoding
            StringSource s(encryptedMSG, true,
                new StreamTransformationFilter(d, new StringSink(plainMSG))); 

            return plainMSG;
        }
        catch (const Exception& e)
        {
            std::cerr << e.what() << std::endl;
            exit(1);
        }
    }

    SecByteBlock AESCustom::getKey()
    {
        return key;
    }

    SecByteBlock AESCustom::getIV()
    {
        return iv;
    }

    void AESCustom::setKey(SecByteBlock newKey)
    {
        key = newKey;
    }

    void AESCustom::setIV(SecByteBlock newIV)
    {
        iv = newIV;
    }

    void AESCustom::setRandomAES()
    {
        // generate key and iv
        randomPool.GenerateBlock(key, key.size());
        randomPool.GenerateBlock(iv, iv.size());
    }

    std::string AESCustom::keyToString(SecByteBlock& key)
    {
        std::string str(reinterpret_cast<const char*>(&key[0]), key.size());
        return str;
    }

    SecByteBlock AESCustom::stringToKey(std::string& str)
    {
        SecByteBlock key(reinterpret_cast<const byte*>(&str[0]), str.size());
        return key;
    }

}

