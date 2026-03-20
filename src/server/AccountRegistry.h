#ifndef ACCOUNT_REGISTRY_H
#define ACCOUNT_REGISTRY_H

#include <map>
#include <string>
#include <optional>

class AccountRegistry
{
    private:
    std::map<std::string, std::string> _accounts;
    int ValidateUsername(const std::string& username);
    int ValidatePassword(const std::string& password);

    public:
    enum class Error : int {
        None = 0, 
        UsernameAlreadyRegistered = 1,
        UsernameUnregistered = 1,
        UsernameUnder8Characters = 2, 
        UsernameOver32Characters = 4, 
        UsernameContainsSpecialCharacters = 8,
        UsernameInvalid = 15, // Aggregate of above
        PasswordUnder12Characters = 16,
        PasswordMissingLowercaseCharacter = 32,
        PasswordMissingUppercaseCharacter = 64,
        PasswordMissingSpecialCharacter = 128,
        PasswordMissingNumber = 256,
        PasswordInvalid = 496, // Aggregate of above
    };
    AccountRegistry() = default;
    int Register(const std::string& username, const std::string& password);
    bool Contains(const std::string& username);
    std::string GetPassword(const std::string& username);
    static std::string ErrorString(int error);

};

#endif