#pragma once
#include <QString>

class PasswordEntry
{
public:
    PasswordEntry() = default;
    PasswordEntry(int id,
        const QString& domain,
        const QString& username,
        const QString& password);

    int id() const;
    QString domain() const;
    QString username() const;
    QString password() const;

    void setId(const int id);
    void setDomain(const QString& domain);
    void setUsername(const QString& username);
    void setPassword(const QString& password);

private:
    int m_id;
    QString m_domain;
    QString m_username;
    QString m_password;
};
