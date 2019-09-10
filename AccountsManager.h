#pragma once
#define FOX_SERVER_ACCOUNTSFILE "Accounts.dat"

#include <string>
#include <vector>
#include <fstream>

//Account data
struct Account
{
    std::string ID, login, password;//data
    unsigned int totalloads;        //total dll load
    int accountban;                //0 = unbanned. 1 = banned
    unsigned int days_remaining;    //sub lenght
    
    Account(std::string id, std::string log1, std::string pas1)
    {
        ID = id;
        login = log1;
        password = pas1;
        accountban = 0;
        days_remaining = 0;
        totalloads = 0;
    }
};

class AFoxAccountsManager
{
private:    
    
    std::vector <Account>cachedAccounts;
public:
    //return = iterator of fox database client. -1 = incorrect password or login. -2 = incorrect ID. 0 >  = iterator 
    int GetIteratorOfAccount(std::string ID, std::string login,std::string password);
    int GetIteratorOfAccount(const char* json_format);
    int GetAdminPanel(const char* json_format);
    
    const char* GetRawJsonInfo(int ACCit);
    
    //return = iterator of account
    int AddAccount(std::string ID, std::string login,std::string password);
    
    //chages to acc
    void BanAccount(int ACCit);
    void RemoveAccount(int ACCit);
    void ChangeDays(int ACCit,unsigned int howmuch);
    void ClearTotalLoads(int ACCit);
    
    //Database manipulation
    void LoadAccountsToCache();
    void FlushAccountsToDatabase();
    
    Account getRaw(int ACCit);
    void DumpToLog();
};

extern AFoxAccountsManager* FoxAccountsManager;