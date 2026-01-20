#include <iostream>
#include "AsymCryptHandler.h"

int main(int argc, char* argv[]) {
    AsymCryptHandler crypt;

    std::cout << "Public Key:  " << crypt.GetPublicKey().GetValueNames() << std::endl;
    std::cout << "Private Key: " << crypt.GetPrivateKey().GetValueNames() << std::endl;

    std::string original = "You're good.";
    std::string cipher = AsymCryptHandler::Encrypt(original, crypt.GetPublicKey());
    std::string deciphered = crypt.Decrypt(cipher);

    std::cout << "Unencrypted: " << original << std::endl;
    std::cout << "Encrypted:   " << cipher << std::endl;
    std::cout << "Decrypted:   " << deciphered << std::endl;

    return 0;
}
