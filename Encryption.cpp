/*
    install: sudo apt-get install libcrypto++-dev libcrypto++-doc libcrypto++-utils
    g++ Encryption.cpp -o Encryption.exe -lcryptopp
*/
#include "encryption.h"

Cipher::Cipher() { 
    AutoSeededRandomPool prng;  
    key  = SecByteBlock (AES::DEFAULT_KEYLENGTH);
    iv = SecByteBlock (AES::BLOCKSIZE);
    prng.GenerateBlock(key, key.size());
    prng.GenerateBlock(iv, iv.size());  
}

std::string Cipher::Encrypt(std::string password) {
    std::string cipher; 
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
    return cipher;
} 

std::string Cipher::Decrypt(std::string cipher) {
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

void Cipher::PrintCipher(std::string cipher) {  
    HexEncoder encoder(new FileSink(std::cout));
    std::cout << "cipher text: ";
    encoder.Put((const byte*)&cipher[0], cipher.size());
    encoder.MessageEnd();
    std::cout << std::endl;
} 

void Cipher::PrintKey() { 
    HexEncoder encoder(new FileSink(std::cout));
    std::cout << "key: ";
    encoder.Put(key, key.size());
    encoder.MessageEnd();
    std::cout << std::endl;
}

/*
int main()
{
    HexEncoder encoder(new FileSink(std::cout));
    std::string password = "CBC Mode Test";  
    Cipher cip;  
    std::string cipher = cip.Encrypt(password);
    cip.PrintKey(); 
    cip.PrintCipher(cipher); 
    std::cout << cip.Decrypt(cipher) << std::endl;
    return 0;
}
*/