#include "server/AccountRegistry.h"
#include "logging/Logger.h"

int AccountRegistry::ValidateUsername(const std::string& username) const
{
    int errors = 0;
    if (_accounts.contains(username))
        errors |= static_cast<int>(Error::UsernameAlreadyRegistered);
    else
    {
        if (username.size() < 8)
            errors |= static_cast<int>(Error::UsernameUnder8Characters);
        else if (username.size() > 32)
            errors |= static_cast<int>(Error::UsernameOver32Characters);

        if (username.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_") != std::string::npos)
            errors |= static_cast<int>(Error::UsernameContainsSpecialCharacters);
    }

    return errors;
}

int AccountRegistry::ValidatePassword(const std::string& password) const
{
    int errors = 0;

    if (password.size() < 12)
        errors |= static_cast<int>(Error::PasswordUnder12Characters);

    if (password.find_first_of("abcdefghijklmnopqrstuvwxyz") == std::string::npos)
        errors |= static_cast<int>(Error::PasswordMissingLowercaseCharacter);

    if (password.find_first_of("ABCDEFGHIJKLMNOPQRSTUVWXYZ") == std::string::npos)
        errors |= static_cast<int>(Error::PasswordMissingUppercaseCharacter);

    if (password.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789") == std::string::npos)
        errors |= static_cast<int>(Error::PasswordMissingSpecialCharacter);

    if (password.find_first_of("0123456789") == std::string::npos)
        errors |= static_cast<int>(Error::PasswordMissingNumber);
    

    return errors;
}

int AccountRegistry::Register(const std::string& username, const std::string& password)
{
    Logger::GetInstance().Info("[AccountRegistry] Attempting to register \"" + username + "\" with password \"" + password + "\"");

    int errors = ValidateUsername(username) | ValidatePassword(password);
    if (errors == static_cast<int>(Error::None))
    {
        _accounts.emplace(username, password);
    }
    else
    {
        Logger::GetInstance().Error("[AccountRegistery]\n" + ErrorString(errors));
    }

    return errors;
}

bool AccountRegistry::Contains(const std::string& username) const
{
    return _accounts.contains(username);
}

std::string AccountRegistry::GetPassword(const std::string& username) const
{
    return _accounts.at(username);
}

std::string AccountRegistry::ErrorString(int error)
{
    std::string output;

    if (error & static_cast<int>(Error::UsernameAlreadyRegistered))
        output += "Username already registered\n";
    if (error & static_cast<int>(Error::UsernameUnder8Characters))
        output += "Username under 8 characters\n";
    if (error & static_cast<int>(Error::UsernameOver32Characters))
        output += "Username over 32 characters\n";
    if (error & static_cast<int>(Error::UsernameContainsSpecialCharacters))
        output += "Username contains special character(s)\n";

    if (error & static_cast<int>(Error::PasswordUnder12Characters))
        output += "Password under 12 characters\n";
    if (error & static_cast<int>(Error::PasswordMissingLowercaseCharacter))
        output += "Password missing a lowercase character\n";
    if (error & static_cast<int>(Error::PasswordMissingUppercaseCharacter))
        output += "Password missing an uppercase character\n";
    if (error & static_cast<int>(Error::PasswordMissingSpecialCharacter))
        output += "Password missing a special character\n";
    if (error & static_cast<int>(Error::PasswordMissingNumber))
        output += "Password missing a number\n";

    if (output.size() > 0)
        output.erase(output.end() - 1); // Remove last \n
    return output;
}