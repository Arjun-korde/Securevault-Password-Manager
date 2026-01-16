#include <QCryptographicHash>
#include <QRandomGenerator>

#include "PasswordHasher.h"

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


QByteArray PasswordHasher::computeHash(const QByteArray& input, int iterations)
{
    QByteArray hash = QCryptographicHash::hash(
        input,
        QCryptographicHash::Sha256
    );

    for (int i = 1; i < iterations; ++i) {
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
    result.salt = generateSalt();

    QByteArray input =
        password.toUtf8() + result.salt;

    result.hash = computeHash(
        input,
        ITERATION_COUNT
    );

    return result;
}

bool PasswordHasher::verifyPassword(
    const QString& password,
    const QByteArray& storedHash,
    const QByteArray& storedSalt
)
{
    QByteArray input =
        password.toUtf8() + storedSalt;

    QByteArray computedHash = computeHash(
        input,
        ITERATION_COUNT
    );

    return computedHash == storedHash;
}
