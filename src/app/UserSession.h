#pragma once

#include <optional>
#include "models/user.h"

class UserSession
{
public:
    static UserSession& instance();

    void setCurrentUser(const User& user);
    void resetCurrentUser();

    const User& currentUser() const;

private:
    std::optional<User> m_currentUser;
};
