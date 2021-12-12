#include "SHA256.h"

SHA256Custom::SHA256Custom(std::string msg)
{
    hashOf(msg);
}

std::string SHA256Custom::hashOf(std::string msg)
{
    // prepare data
    std::string hashedMsg;
    unsigned char* byteMsg = (unsigned char*)malloc(msg.size() + 1);
    unsigned char* hashedByteMsg = (unsigned char*)malloc(33*sizeof(char));
    memcpy(byteMsg, msg.c_str(), msg.size());

    // hashing
    SHA256(byteMsg, msg.size(), hashedByteMsg);

    // convert char[] to string
    hashedMsg = reinterpret_cast<char*>(hashedByteMsg);

    // free memory
    free(byteMsg);
    free(hashedByteMsg);

    // finalization
    hashedMsg.resize(32);
    hash = hashedMsg;
    return hash;
}

std::string SHA256Custom::getHash()
{
    return hash;
}

void SHA256Custom::printAsHex()
{
    for (int i = 0; i < hash.length(); i++) {
        printf("%02hhx", hash[i]);
    }
    std::cout << std::endl;
}

void SHA256Custom::printAsChar()
{
    std::cout << hash << std::endl;
}

bool SHA256Custom::isEqual(SHA256Custom hash1, SHA256Custom hash2)
{
    return isEqual(hash1.getHash(), hash2.getHash());
}

bool SHA256Custom::isEqual(std::string hash1, std::string hash2)
{
    return !hash1.compare(hash2);
}
