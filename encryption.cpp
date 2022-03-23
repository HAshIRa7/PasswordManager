#include "encryption.h"


#ifndef STREAM
    #define STREAM
    #include <QTextStream>
    QTextStream cout3(stdout);
    QTextStream cin3(stdin);
    QTextStream cerr3(stderr);
#endif


Cipher::Cipher(std::string password) {
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

std::string Cipher::Decrypt() {
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

void Cipher::PrintCipher() {
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
