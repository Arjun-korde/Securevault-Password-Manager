#pragma once

#include <QByteArray>

/*
 * CryptoManager
 *
 * Responsibility:
 *  - Symmetric encryption/decryption of secrets
 *  - AES-256-CBC (temporary, upgradeable to GCM)
 *
 * Notes:
 *  - Caller provides the encryption key
 *  - IV is generated per encryption
 *  - No authentication yet (CBC limitation)
 */

class CryptoManager
{
public:
    struct EncryptedData {
        QByteArray cipherText;
        QByteArray iv;
    };

    static EncryptedData encrypt(
        const QByteArray& plainText,
        const QByteArray& key
    );

    static QByteArray decrypt(
        const QByteArray& cipherText,
        const QByteArray& iv,
        const QByteArray& key
    );

private:
    static QByteArray generateRandomBytes(int size);
};
