#include "UserRepository.h"

#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

std::optional<User> UserRepository::login(const QString& username, const QString& password)
{
    QSqlQuery query(DatabaseManager::instance().database());
    query.prepare(R"(
        SELECT id, username, password_hash, password_salt FROM users
        WHERE username = :username
    )");
    query.bindValue(":username", normalizeUsername(username));

    if (!query.exec()) {
        qDebug() << "error occured while login : " << query.lastError();
        return std::nullopt;
    }

    if (!query.next()) {
        qDebug() << "username not found";
        return std::nullopt;
    }

    const QByteArray storedHash = query.value("password_hash").toByteArray();
    const QByteArray storedSalt = query.value("password_salt").toByteArray();

    const bool isValidPassword = PasswordHasher::verifyPassword(
        password, storedHash, storedSalt
    );

    if (!isValidPassword) {
        return std::nullopt;
    }

    return User(
        query.value("id").toInt(),
        query.value("username").toString()
    );
}

bool UserRepository::createUser(const QString& username, const QString& password)
{
    auto hashResult = PasswordHasher::hashPassword(password);

    QSqlQuery query(DatabaseManager::instance().database());
    query.prepare(R"(
        INSERT INTO users (username, password_hash, password_salt)
        VALUES (:username, :password_hash, :password_salt)
    )");

    query.bindValue(":username", normalizeUsername(username));
    query.bindValue(":password_hash", hashResult.hash);
    query.bindValue(":password_salt", hashResult.salt);

    if (!query.exec()) {
        qDebug() << "error occured while creating User" << query.lastError();
        return false;
    }

    return true;
}

bool UserRepository::deleteAccount(int userId, const QString& password)
{
    QSqlQuery query(DatabaseManager::instance().database());
    query.prepare(R"(
        DELETE FROM users 
        WHERE id = :userId AND password_hash = :password
    )");
    query.bindValue(":userId", userId);
    query.bindValue(":password", password);

    if (!query.exec()) {
        qDebug() << "error occured while deleting user : " << query.lastError();
        return false;
    }

    return query.numRowsAffected() > 0;
}

QString UserRepository::normalizeUsername(const QString& username) {
    return username.trimmed().toLower();
}