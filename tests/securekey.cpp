#include "Crypto/KeyAgreement.h"
#include "Crypto/SecureKey.h"
#include <cstring>
#include <cassert>

int main()
{
    KeyAgreement agree_a;
    KeyAgreement agree_b;
    {   
        agree_a.ResizeSalt(256);
        agree_b.ResizeSalt(256);
        agree_a.GenerateSalt();
        uint8_t *data_a, *data_b;
        std::size_t size_a, size_b;
        agree_a.GetSalt(data_a, size_a);
        agree_b.GetSalt(data_b, size_b);
        assert(256 == size_a);
        assert(size_a == size_b);
        memcpy(data_b, data_a, size_a);
    }
    {   
        uint8_t *data_a, *data_b;
        std::size_t size_a, size_b;
        agree_a.GetOtherKey(data_a, size_a);
        agree_b.GetPublicKey(data_b, size_b);
        assert(size_a == size_b);
        memcpy(data_a, data_b, size_a);
    }
    {   
        uint8_t *data_a, *data_b;
        std::size_t size_a, size_b;
        agree_a.GetPublicKey(data_a, size_a);
        agree_b.GetOtherKey(data_b, size_b);
        assert(size_a == size_b);
        memcpy(data_b, data_a, size_a);
    }

    auto result_a = agree_a.Agree();
    auto result_b = agree_b.Agree();
    if (!result_a.has_value() || !result_b.has_value()) {
        return 1;
    }

    SecureKey key_a(std::move(*result_a));
    SecureKey key_b(std::move(*result_b));

    uint8_t iv[SecureKey::BlockSize];
    std::size_t buf_size = 1 * SecureKey::BlockSize;
    char* plaintext_a = new char[buf_size];
    char* encrypted   = new char[buf_size];
    char* plaintext_b = new char[buf_size];

    strcpy(plaintext_a, "Hello world!");
    key_a.Encrypt(encrypted, plaintext_a, 1, iv);
    assert(memcmp(encrypted, plaintext_a, buf_size) != 0);
    key_b.Decrypt(plaintext_b, encrypted, 1, iv);
    assert(memcmp(plaintext_a, plaintext_b, buf_size) == 0);

    delete[] plaintext_a;
    delete[] encrypted;
    delete[] plaintext_b;

    return 0;
}