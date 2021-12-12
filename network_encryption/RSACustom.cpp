#include "RSACustom.h"

#include <boost/random/mersenne_twister.hpp>

namespace rsa {

    RSACustom::RSACustom() {
        //srand((unsigned)time(NULL)); // fully random on each run
        generateKeyPair();
    }

    cpp_int RSACustom::encrypt(std::string plainMSG)
    {
        if (remotePublicKey.exp == 0 || remotePublicKey.modulus == 0) {
            cons::print("[ERROR] Remote public key is not set!");
            exit(1);
        }

        // write bytes from string to cpp_int
        cpp_int decryptedMSG = 0;
        for (size_t i = 0; i < plainMSG.size(); i++) {
            decryptedMSG = decryptedMSG << 8;
            decryptedMSG += static_cast<unsigned char>(plainMSG[i]);
        }

        // encrypt
        cpp_int encryptedMSG = fastExp(decryptedMSG, remotePublicKey.exp, remotePublicKey.modulus);

        return encryptedMSG;
    }

    cpp_int RSACustom::encrypt(cpp_int plainMSG)
    {
        if (remotePublicKey.exp == 0 || remotePublicKey.modulus == 0) {
            cons::print("[ERROR] Remote public key is not set!");
            exit(1);
        }

        // encrypt
        cpp_int encryptedMSG = fastExp(plainMSG, remotePublicKey.exp, remotePublicKey.modulus);

        return encryptedMSG;
    }

    cpp_int RSACustom::encryptSign(std::string plainMSG)
    {

        // write bytes from string to cpp_int
        cpp_int decryptedMSG = 0;
        for (size_t i = 0; i < plainMSG.size(); i++) {
            decryptedMSG = decryptedMSG << 8;
            decryptedMSG += static_cast<unsigned char>(plainMSG[i]);
        }

        // encrypt
        cpp_int encryptedMSG = fastExp(decryptedMSG, privateKey.exp, privateKey.modulus);

        return encryptedMSG;
    }

    cpp_int RSACustom::encryptSign(cpp_int plainMSG)
    {
        // encrypt
        cpp_int encryptedMSG = fastExp(plainMSG, privateKey.exp, privateKey.modulus);

        return encryptedMSG;
    }

    std::string RSACustom::decrypt(cpp_int encryptedMSG)
    {
        // decrypt
        cpp_int decryptedMSG = fastExp(encryptedMSG, privateKey.exp, privateKey.modulus);

        // write decrypted msg to string
        std::string plainMSG;
        while (decryptedMSG > 0) {
            plainMSG.push_back(static_cast<unsigned char>(decryptedMSG % 256));
            decryptedMSG = decryptedMSG >> 8;
        }

        // reverse string
        std::string temp = plainMSG;
        for (size_t i = 0; i < temp.size(); i++) {
            plainMSG[plainMSG.size() - i - 1] = temp[i];
        }

        return plainMSG;
    }

    std::string RSACustom::decryptSign(cpp_int encryptedMSG)
    {
        if (remotePublicKey.exp == 0 || remotePublicKey.modulus == 0) {
            cons::print("[ERROR] Remote public key is not set!");
            exit(1);
        }

        // decrypt
        cpp_int decryptedMSG = fastExp(encryptedMSG, remotePublicKey.exp, remotePublicKey.modulus);

        // write decrypted msg to string
        std::string plainMSG;
        while (decryptedMSG > 0) {
            plainMSG.push_back(static_cast<unsigned char>(decryptedMSG % 256));
            decryptedMSG = decryptedMSG >> 8;
        }

        // reverse string
        std::string temp = plainMSG;
        for (size_t i = 0; i < temp.size(); i++) {
            plainMSG[plainMSG.size() - i - 1] = temp[i];
        }

        return plainMSG;
    }

    std::string RSACustom::decryptAndUnsign(cpp_int encryptedMSG)
    {

        if (remotePublicKey.exp == 0 || remotePublicKey.modulus == 0) {
            cons::print("[ERROR] Remote public key is not set!");
            exit(1);
        }

        // decrypt
        cpp_int decryptedMSG = fastExp(encryptedMSG, privateKey.exp, privateKey.modulus);

        // unsign
        decryptedMSG = fastExp(decryptedMSG, remotePublicKey.exp, remotePublicKey.modulus);

        // write decrypted msg to string
        std::string plainMSG;
        while (decryptedMSG > 0) {
            plainMSG.push_back(static_cast<unsigned char>(decryptedMSG % 256));
            decryptedMSG = decryptedMSG >> 8;
        }

        // reverse string
        std::string temp = plainMSG;
        for (size_t i = 0; i < temp.size(); i++) {
            plainMSG[plainMSG.size() - i - 1] = temp[i];
        }

        return plainMSG;
    }

    void RSACustom::setRemotePublicKey(Key newRemotePublicKey)
    {
        remotePublicKey = newRemotePublicKey;
    }

    Key RSACustom::getPublicKey()
    {
        return publicKey;
    }

    void RSACustom::generateKeyPair()
    {
        //cons::print("[RSA] Key generation started...", YELLOW);

        // get p, q
        cpp_int p = getPrime();
        cpp_int q = getPrime();
        // regenerate in case of equality
        while (p == q) {
            q = getPrime();
        }

        // modulus found
        cpp_int n = p * q;
        publicKey.modulus = n;
        privateKey.modulus = n;

        // calculate the totient fi(n)
        cpp_int fi = (p - 1) * (q - 1);

        // get e from define
        cpp_int e = PUBLIC_KEY_EXP;
        publicKey.exp = e;

        // calc d with table method
        // table creation
        std::vector<cpp_int> Q;
        std::vector<std::vector<cpp_int> > u, v, T;

        // calc
        u.resize(1);
        u[0].resize(3);
        u[0] = { fi, 1, 0 };
        v.resize(1);
        v[0].resize(3);
        v[0] = { e, 0, 1 };
        while (v[v.size() - 1][0] != 0) {
            Q.resize(Q.size() + 1);
            Q[Q.size() - 1] = u[u.size() - 1][0] / v[v.size() - 1][0];
            T.resize(T.size() + 1);
            T[T.size() - 1] = { u[u.size() - 1][0] % v[v.size() - 1][0],
                u[u.size() - 1][1] - Q[Q.size() - 1] * v[v.size() - 1][1],
                u[u.size() - 1][2] - Q[Q.size() - 1] * v[v.size() - 1][2] };

            u.resize(u.size() + 1);
            u[u.size() - 1] = v[v.size() - 1];
            v.resize(v.size() + 1);
            v[v.size() - 1] = T[T.size() - 1];
        }

        // result
        cpp_int d = fi + u[u.size() - 1][2];
        privateKey.exp = d;

        //cons::print("[RSA] Key generation ended! [p = " + cons::toString(p) + ", q = " + cons::toString(q) + ", n = " + cons::toString(n)
        //    + ", e = " + cons::toString(e) + ", d = " + cons::toString(d) + "]", GREEN);
    }

    cpp_int RSACustom::extendedEuclid(cpp_int e, cpp_int fi, cpp_int& d, cpp_int& y)
    {
        if (e == 0) {
            d = 0; y = 1;
            return fi;
        }
        cpp_int d1, y1;
        cpp_int tempGCD = extendedEuclid(fi % e, e, d1, y1);
        d = y1 - (fi / e) * d1;
        y = d1;
        return tempGCD;
    }

    cpp_int RSACustom::getPrime()
    {
        // base number generation 
        constexpr int bitsCount = RSA_KEY_SIZE; // how much bits the RSA key will have
        mt11213b base_gen(rand());
        independent_bits_engine<mt11213b, bitsCount / 2, cpp_int> gen(base_gen);

        // regenerate number until it pass
        cpp_int n = gen();
        while (!miller_rabin_test(n, 25)) { // with 25 trials of Miller - Rabin number is LIKELY prime
            n = gen();
        }
        return n;
    }

    std::string RSACustom::toString(cpp_int cppInt)
    {
        std::string tempString;
        while (cppInt > 0) {
            tempString.push_back(static_cast<unsigned char>(cppInt % 256));
            cppInt = cppInt >> 8;
        }

        // reverse string
        std::string temp = tempString;
        for (size_t i = 0; i < temp.size(); i++) {
            tempString[tempString.size() - i - 1] = temp[i];
        }

        return tempString;
    }

    cpp_int RSACustom::toCppInt(std::string text)
    {
        cpp_int tempCppInt = 0;
        for (size_t i = 0; i < text.size(); i++) {
            tempCppInt = tempCppInt << 8;
            tempCppInt += static_cast<unsigned char>(text[i]);
        }
        return tempCppInt;
    }

    cpp_int RSACustom::fastExp(cpp_int x, cpp_int y, cpp_int N)
    {
        if (y == 0) return 1;
        cpp_int z = fastExp(x, y / 2, N);
        if (y % 2 == 0)
            return (z * z) % N;
        else
            return (x * z * z) % N;
    }

}


