#include <iostream>
#include "SymCryptHandler.h"

int main(int argc, char* argv[]) {
    auto sharedKey = SymCryptHandler::KeyFromSecret("Love and Peace");
    std::cout << "Shared Private Key: ";
    for (auto bte : sharedKey)
    {
        std::cout << bte;
    }
    std::cout << std::endl;
    SymCryptHandler alice (sharedKey);
    SymCryptHandler bob ("Love and Peace");

    std::cout << "Alice's Key: " << SymCryptHandler::KeyToString(alice.GetKey()) << std::endl;
    std::cout << "Bob's Key:   " << SymCryptHandler::KeyToString(bob.GetKey()) << std::endl;

    std::string original = "You're good.";
    std::string cipher = alice.Encrypt(original);
    std::string deciphered = bob.Decrypt(cipher);

    std::cout << "Unencrypted: " << original << std::endl;
    std::cout << "Encrypted:   " << cipher << std::endl;
    std::cout << "Decrypted:   " << deciphered << std::endl;

    return 0;
}
