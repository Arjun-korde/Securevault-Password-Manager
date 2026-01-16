#include<QDebug>

#include "usersession.h"

UserSession& UserSession::instance()
{
    static UserSession instance;
    return instance;
}

void UserSession::setCurrentUser(const User& user)
{
    m_currentUser = user;
    qDebug() << "Currently logged in user : id=" << user.id() << " username=" << user.username();
}

void UserSession::resetCurrentUser()
{
    qDebug() << "user logged out : id=" << m_currentUser->id() << " username=" << m_currentUser->username();
    m_currentUser.reset();
}

const User& UserSession::currentUser() const
{
    return m_currentUser.value();
}
