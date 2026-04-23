#include "Crypto/KeyAgreement.h"
#include "Crypto/SecureKey.h"
#include <cstring>
#include <cassert>

int main()
{
    KeyAgreement agree_a;
    KeyAgreement agree_b;
    {   
        // Salt
        agree_a.GenerateSalt(256);
        uint8_t *data_a, *data_b;
        std::size_t size_a, size_b;
        agree_a.GetSalt(data_a, size_a);
        agree_b.GetSalt(data_b, size_b);
        assert(256 == size_a && size_a == size_b);
        memcpy(data_b, data_a, size_a);
    }
    {   
        // A Other
        uint8_t *data_a, *data_b;
        std::size_t size_a, size_b;
        agree_a.GetOtherKey(data_a, size_a);
        agree_b.GetPublicKey(data_b, size_b);
        assert(size_a == size_b);
        memcpy(data_a, data_b, size_a);

    }
    {   
        // B Other
        uint8_t *data_a, *data_b;
        std::size_t size_a, size_b;
        agree_a.GetPublicKey(data_a, size_a);
        agree_b.GetOtherKey(data_b, size_b);
        assert(size_a == size_b);
        memcpy(data_b, data_a, size_a);
    }

    SecureKey key_a (agree_a.Agree().value());
    SecureKey key_b (agree_b.Agree().value());
    uint8_t iv[SecureKey::IV_LENGTH];

    char plaintext_a[13] = "Hello world!";
    char encrypted[13] = "";
    char plaintext_b[13] = "";
    key_a.Encrypt(encrypted, plaintext_a, 13, iv);
    assert(strcmp(encrypted, plaintext_a) != 0);
    key_b.Decrypt(plaintext_b, encrypted, 13, iv);
    assert(strcmp(plaintext_a, plaintext_b) == 0);

    return 0;
}