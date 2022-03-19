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
    Cipher(std::string password) { 
        AutoSeededRandomPool prng;  
        key  = SecByteBlock (AES::DEFAULT_KEYLENGTH);
        iv = SecByteBlock (AES::BLOCKSIZE);
        prng.GenerateBlock(key, key.size());
        prng.GenerateBlock(iv, iv.size()); 

        try
        {
            CBC_Mode< AES >::Encryption e;
            e.SetKeyWithIV(key, key.size(), iv);

            StringSource s(password, true, 
                new StreamTransformationFilter(e,
                    new StringSink(cipher)
                ) // StreamTransformationFilter
            ); // StringSource
        }
        catch(const Exception& e)
        {
            std::cerr << e.what() << std::endl;
            exit(1);
        }
    } 
    
    std::string Decrypt() {
        std::string recovered;  
        try
        {
            CBC_Mode< AES >::Decryption d;
            d.SetKeyWithIV(key, key.size(), iv);

            StringSource s(cipher, true, 
                new StreamTransformationFilter(d,
                    new StringSink(recovered)
                ) // StreamTransformationFilter
            ); // StringSource
        }
        catch(const Exception& e)
        {
            std::cerr << e.what() << std::endl;
            exit(1);
        }
        return recovered; 
    }  

    void PrintCipher() {  
        HexEncoder encoder(new FileSink(std::cout));
        std::cout << "cipher text: ";
        encoder.Put((const byte*)&cipher[0], cipher.size());
        encoder.MessageEnd();
        std::cout << std::endl;
    } 

    void PrintKey() { 
        HexEncoder encoder(new FileSink(std::cout));
        std::cout << "key: ";
        encoder.Put(key, key.size());
        encoder.MessageEnd();
        std::cout << std::endl;
    }
    ~Cipher() {} 
private:   
    SecByteBlock key; 
    SecByteBlock iv;  
    std::string cipher; 
}; 

int main()
{
    HexEncoder encoder(new FileSink(std::cout));
    std::string password = "CBC Mode Test";  
    Cipher cip(password); 
    cip.PrintKey(); 
    cip.PrintCipher(); 
    std::cout << cip.Decrypt() << std::endl;
    return 0;
}