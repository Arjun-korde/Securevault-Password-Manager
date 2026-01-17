#pragma once

#include <QString>
#include <QByteArray>

/*
 * KeyDerivation
 *
 * Responsibility:
 *  - Derive a deterministic AES-256 encryption key
 *    from the user's master password.
 *
 * Design notes:
 *  - Uses a dedicated encryption salt (NOT password-hash salt)
 *  - Key is never stored or persisted
 *  - Caller controls key lifetime (usually UserSession)
 *  - No UI, DB, or session dependencies
 */

class KeyDerivation
{
public:
    // Generates a random salt for encryption key derivation
    // This should be generated once per user and stored in DB
    static QByteArray generateEncryptionSalt();

    // Derives a 256-bit (32-byte) AES key
    static QByteArray deriveEncryptionKey(
        const QString& masterPassword,
        const QByteArray& encryptionSalt
    );

private:
    static QByteArray computeHash(const QByteArray& input);
};
