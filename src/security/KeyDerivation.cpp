#include "KeyDerivation.h"
#include "SecurityConfig.h"

#include <QCryptographicHash>
#include <QRandomGenerator>

QByteArray KeyDerivation::generateEncryptionSalt()
{
    QByteArray salt(
        SecurityConfig::ENCRYPTION_SALT_SIZE_BYTES,
        Qt::Uninitialized
    );

    QRandomGenerator* rng = QRandomGenerator::system();

    for (int i = 0; i < SecurityConfig::ENCRYPTION_SALT_SIZE_BYTES; ++i) {
        salt[i] = static_cast<char>(rng->generate() & 0xFF);
    }

    return salt;
}

QByteArray KeyDerivation::computeHash(const QByteArray& input)
{
    QByteArray hash = QCryptographicHash::hash(
        input,
        QCryptographicHash::Sha256
    );

    for (int i = 1; i < SecurityConfig::KEY_DERIVATION_ITERATIONS; ++i) {
        hash = QCryptographicHash::hash(
            hash,
            QCryptographicHash::Sha256
        );
    }

    return hash;
}

QByteArray KeyDerivation::deriveEncryptionKey(
    const QString& masterPassword,
    const QByteArray& encryptionSalt
)
{
    QByteArray input =
        masterPassword.toUtf8() + encryptionSalt;

    QByteArray derived = computeHash(input);

    // SHA-256 already gives 32 bytes, but we enforce size explicitly
    return derived.left(SecurityConfig::AES_KEY_SIZE_BYTES);
}
