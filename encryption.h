#pragma once

#include "cryptopp/cryptlib.h"
#include "cryptopp/rijndael.h"
#include "cryptopp/modes.h"
#include "cryptopp/files.h"
#include "cryptopp/osrng.h"
#include "cryptopp/hex.h"

#include <iostream>
#include <string> 

using namespace CryptoPP;

class Cipher {
public:
    Cipher();
    std::string Encrypt(std::string password);
    std::string Decrypt(std::string cipher);  
    void PrintCipher(std::string cipher);
    void PrintKey();
    ~Cipher() {} 
private:   
    SecByteBlock key; 
    SecByteBlock iv;
}; 