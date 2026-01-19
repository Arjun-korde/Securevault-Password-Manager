#pragma once

#include <QByteArray>
#include <QString>
#include <cstdint>

namespace CryptoUtils {

    QByteArray generateRandomBytes(size_t size);

    /*
     * PBKDF2-HMAC-SHA256
     * - password: raw bytes (UTF-8 handled by caller)
     * - salt: random bytes
     * - iterations: must be >= 100k for passwords
     * - outputLen: derived key length in bytes
     */
    QByteArray pbkdf2Sha256(
        const QByteArray& password,
        const QByteArray& salt,
        uint32_t iterations,
        size_t outputLen
    );

} // namespace CryptoUtils
