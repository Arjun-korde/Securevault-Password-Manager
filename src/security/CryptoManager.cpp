#include "CryptoManager.h"

#include <windows.h>
#include <bcrypt.h>

#pragma comment(lib, "bcrypt.lib")

#define NT_SUCCESS(Status) (((NTSTATUS)(Status)) >= 0)

namespace {
    constexpr int AES_KEY_SIZE = 32;
    constexpr int AES_BLOCK_SIZE = 16;
}

QByteArray CryptoManager::generateRandomBytes(int size)
{
    QByteArray buffer(size, Qt::Uninitialized);

    BCryptGenRandom(
        nullptr,
        reinterpret_cast<PUCHAR>(buffer.data()),
        size,
        BCRYPT_USE_SYSTEM_PREFERRED_RNG
    );

    return buffer;
}

CryptoManager::EncryptedData
CryptoManager::encrypt(const QByteArray& plainText, const QByteArray& key)
{
    BCRYPT_ALG_HANDLE hAlg = nullptr;
    BCRYPT_KEY_HANDLE hKey = nullptr;

    DWORD cbData = 0;
    DWORD cbCipher = 0;

    EncryptedData result;
    result.iv = generateRandomBytes(AES_BLOCK_SIZE);

    BCryptOpenAlgorithmProvider(
        &hAlg,
        BCRYPT_AES_ALGORITHM,
        nullptr,
        0
    );

    BCryptSetProperty(
        hAlg,
        BCRYPT_CHAINING_MODE,
        (PUCHAR)BCRYPT_CHAIN_MODE_CBC,
        sizeof(BCRYPT_CHAIN_MODE_CBC),
        0
    );

    BCryptGenerateSymmetricKey(
        hAlg,
        &hKey,
        nullptr,
        0,
        (PUCHAR)key.data(),
        AES_KEY_SIZE,
        0
    );

    // Get output size
    BCryptEncrypt(
        hKey,
        (PUCHAR)plainText.data(),
        plainText.size(),
        nullptr,
        (PUCHAR)result.iv.data(),
        result.iv.size(),
        nullptr,
        0,
        &cbCipher,
        BCRYPT_BLOCK_PADDING
    );

    result.cipherText.resize(cbCipher);

    QByteArray iv = result.iv;

    BCryptEncrypt(
        hKey,
        (PUCHAR)plainText.data(),
        plainText.size(),
        nullptr,
        (PUCHAR)iv.data(),
        result.iv.size(),
        (PUCHAR)result.cipherText.data(),
        cbCipher,
        &cbCipher,
        BCRYPT_BLOCK_PADDING
    );

    BCryptDestroyKey(hKey);
    BCryptCloseAlgorithmProvider(hAlg, 0);

    return result;
}

QByteArray CryptoManager::decrypt(
    const QByteArray& cipherText,
    const QByteArray& iv,
    const QByteArray& key
)
{
    BCRYPT_ALG_HANDLE hAlg = nullptr;
    BCRYPT_KEY_HANDLE hKey = nullptr;

    DWORD cbPlain = 0;
    QByteArray plainText;

    BCryptOpenAlgorithmProvider(
        &hAlg,
        BCRYPT_AES_ALGORITHM,
        nullptr,
        0
    );

    BCryptSetProperty(
        hAlg,
        BCRYPT_CHAINING_MODE,
        (PUCHAR)BCRYPT_CHAIN_MODE_CBC,
        sizeof(BCRYPT_CHAIN_MODE_CBC),
        0
    );

    BCryptGenerateSymmetricKey(
        hAlg,
        &hKey,
        nullptr,
        0,
        (PUCHAR)key.data(),
        AES_KEY_SIZE,
        0
    );

    BCryptDecrypt(
        hKey,
        (PUCHAR)cipherText.data(),
        cipherText.size(),
        nullptr,
        (PUCHAR)iv.data(),
        iv.size(),
        nullptr,
        0,
        &cbPlain,
        BCRYPT_BLOCK_PADDING
    );

    plainText.resize(cbPlain);

    BCryptDecrypt(
        hKey,
        (PUCHAR)cipherText.data(),
        cipherText.size(),
        nullptr,
        (PUCHAR)iv.data(),
        iv.size(),
        (PUCHAR)plainText.data(),
        cbPlain,
        &cbPlain,
        BCRYPT_BLOCK_PADDING
    );

    plainText.resize(cbPlain);

    BCryptDestroyKey(hKey);
    BCryptCloseAlgorithmProvider(hAlg, 0);

    return plainText;
}
