#ifndef ACCOUNT_REGISTRY_H
#define ACCOUNT_REGISTRY_H

#include <map>
#include <string>
#include <optional>

class AccountRegistry
{
    private:
    std::map<std::string, std::string> _accounts;
    int ValidateUsername(const std::string& username) const;
    int ValidatePassword(const std::string& password) const;
    std::string HashPassword(const std::string& password) const;
    bool SaveAccount(const std::string& username, const std::string& filepath);
    bool DeleteAccount(const std::string& username, const std::string& filepath);
    bool LoadAccounts(const std::string& filepath);

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
    bool Contains(const std::string& username) const;
    bool MatchingPassword(const std::string& username, const std::string& password) const;
    static std::string ErrorString(int error);

};

#endif