#pragma once

#include <QString>
#include <QByteArray>

/*
 * PasswordHasher
 *
 * Responsibility:
 *  - Hash master passwords securely
 *  - Verify passwords during login
 *
 * Notes:
 *  - Uses salted + iterated SHA-256
 *  - Iteration count is global (not stored in DB)
 *  - Does NOT depend on UI, DB, or session state
 */

class PasswordHasher
{
public:
    struct HashResult {
        QByteArray hash;
        QByteArray salt;
    };

    // Hash a plain-text password (used during signup)
    static HashResult hashPassword(const QString& password);

    // Verify a password against stored hash + salt (used during login)
    static bool verifyPassword(
        const QString& password,
        const QByteArray& storedHash,
        const QByteArray& storedSalt
    );

private:
    static QByteArray generateSalt(int lengthBytes = 16);
    static QByteArray computeHash(
        const QByteArray& input,
        int iterations
    );

private:
    static constexpr int ITERATION_COUNT = 1'00'000;
};
