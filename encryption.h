#pragma once

#include "cryptopp/cryptlib.h"
#include "cryptopp/rijndael.h"
#include "cryptopp/modes.h"
#include "cryptopp/files.h"
#include "cryptopp/osrng.h"
#include "cryptopp/hex.h"

#include <iostream>
#include <string>
//#include "cionout.h"
using namespace CryptoPP;

class Cipher {
public:
    Cipher(std::string password);
    std::string Decrypt();
    void PrintCipher();
    void PrintKey();
    ~Cipher() {}
private:
    SecByteBlock key;
    SecByteBlock iv;
    std::string cipher;
};
