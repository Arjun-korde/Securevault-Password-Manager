#include <QCryptographicHash>
#include <QRandomGenerator>

#include "PasswordHasher.h"
#include "CryptoUtils.h"

QByteArray PasswordHasher::generateSalt(int lengthBytes)
{
    QByteArray salt;
    salt.resize(lengthBytes);

    QRandomGenerator* rng = QRandomGenerator::system();

    for (int i = 0; i < lengthBytes; ++i) {
        salt[i] = static_cast<char>(rng->generate() & 0xFF);
    }
 
    return salt;
}

QByteArray PasswordHasher::computeHash(const QByteArray& input)
{
    QByteArray hash = QCryptographicHash::hash(
        input,
        QCryptographicHash::Sha256
    );

    for (int i = 1; i < SecurityConfig::PASSWORD_HASH_ITERATIONS; ++i) {
        hash = QCryptographicHash::hash(
            hash,
            QCryptographicHash::Sha256
        );
    }

    return hash;
}

PasswordHasher:: HashResult 
PasswordHasher::hashPassword(const QString& password)
{
    HashResult result;
    if (password.isEmpty()) {
        return {};
    }

    result.salt = CryptoUtils::generateRandomBytes(SecurityConfig::PASSWORD_SALT_SIZE_BYTES);

    if (result.salt.isEmpty()) {
        return {};
    }

    QByteArray passwordBytes = password.toUtf8();

    result.hash = CryptoUtils::pbkdf2Sha256(
        passwordBytes,
        result.salt,
        SecurityConfig::PASSWORD_HASH_ITERATIONS,
        SecurityConfig::PASSWORD_HASH_SIZE_BYTES
    );

    if (result.hash.isEmpty()) {
        return {};
    }

    return result;
}

bool PasswordHasher::verifyPassword(
    const QString& password,
    const QByteArray& storedHash,
    const QByteArray& storedSalt
)
{
    QByteArray passwordBytes = password.toUtf8();

    QByteArray computedHash = CryptoUtils::pbkdf2Sha256(
        passwordBytes,
        storedSalt,
        SecurityConfig::PASSWORD_HASH_ITERATIONS,
        SecurityConfig::PASSWORD_HASH_SIZE_BYTES
    );

    return computedHash == storedHash;
}
