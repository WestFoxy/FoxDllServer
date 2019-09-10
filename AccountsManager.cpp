#include "AccountsManager.h"
#include "includes.h"
#include "Log.h"
#include "json.hpp"

using json = nlohmann::json;

AFoxAccountsManager* FoxAccountsManager = new AFoxAccountsManager;

using namespace std;

int AFoxAccountsManager::GetIteratorOfAccount(std::string ID, std::string login,std::string password)
{
    //parse cache
    for(int i = 0; i < this->cachedAccounts.size(); i++)
    {
        if(!cachedAccounts[i].login.compare(login) && !cachedAccounts[i].password.compare(password))
        {
            
            if(!cachedAccounts[i].ID.compare(ID))
            {
                FoxLogger->AppendInfo("L: %s P:%s ID: %s", login.c_str(), password.c_str(), ID.c_str());
                return i; //iterator of account
            }
            else
            {
                FoxLogger->AppendWarning("INVALID HWID. Login: %s. Password: %s. ID: %s", login.c_str(), password.c_str(), ID.c_str());
                return -2; //INcorrect hardware ID
                
            }
            
        }
    }
    FoxLogger->AppendWarning("Invalid password or log. L: %s. P: %s", login.c_str(), password.c_str());
    return -1; //Not in database/ incorrect pass or login
}

int AFoxAccountsManager::GetAdminPanel(const char* json_format)
{
    try{
        json AccountData = json::parse(json_format);
        string uLogin = AccountData["Login"];
        string uPassword = AccountData["Password"];
        
        if(!uLogin.compare("LovelyFoxFox") && !uPassword.compare("FoxIsLove9380"))
            return 666;
        return -1;
    
    }
    catch(std::invalid_argument ar)
    {
        FoxLogger->AppendError("Admin account parsing error: %s", ar.what());
        return -3;
    }
}

int AFoxAccountsManager::GetIteratorOfAccount(const char* json_format)
{
    try{
        json AccountData = json::parse(json_format);
        string uLogin = AccountData["Login"];
        string uPassword = AccountData["Password"];
        string uID = AccountData["ID"];
        
        return GetIteratorOfAccount(uID, uLogin, uPassword);
    
    }
    catch(std::invalid_argument ar)
    {
        FoxLogger->AppendError("Account parsing error: %s", ar.what());
        return -3;
    }
    
}

const char* AFoxAccountsManager::GetRawJsonInfo(int ACCit)
{
    char buffer[4096];
    json tmp_json;
    
    if(ACCit > cachedAccounts.size() || ACCit < 0)
        return 0;
        
    try{
        tmp_json["Ban"] = cachedAccounts[ACCit].accountban;
        tmp_json["Subscription"] = cachedAccounts[ACCit].days_remaining;
        tmp_json["TotalLoaded"] = cachedAccounts[ACCit].totalloads;
        
        return tmp_json.dump().c_str();
    }
    catch(std::invalid_argument ar)
    {
        FoxLogger->AppendError("GetRawJsonInfo: %s", ar.what());
    }
    
    
}

int AFoxAccountsManager::AddAccount(std::string ID, std::string login,std::string password)
{
    Account temp(ID, login, password);
    
    cachedAccounts.push_back(temp);
    return (cachedAccounts.size()-1); //last added acc
}

void AFoxAccountsManager::BanAccount(int ACCit)
{
    if(ACCit < cachedAccounts.size())
        cachedAccounts[ACCit].accountban = true;
}

void AFoxAccountsManager::RemoveAccount(int ACCit)
{
    if(ACCit < cachedAccounts.size())
        cachedAccounts.erase(cachedAccounts.begin()+ACCit);
}

void AFoxAccountsManager::ChangeDays(int ACCit,unsigned int howmuch)
{
     if(ACCit < cachedAccounts.size())
        cachedAccounts[ACCit].days_remaining = howmuch;
}
void AFoxAccountsManager::ClearTotalLoads(int ACCit)
{
    if(ACCit < cachedAccounts.size())
        cachedAccounts[ACCit].totalloads = 0;
}

void AFoxAccountsManager::LoadAccountsToCache()
{
    std::ifstream inp(FOX_SERVER_ACCOUNTSFILE);
    if(inp.is_open())
    {
        cachedAccounts.clear();
        Account tempaccount("", "", "");
        string tmpline;
        int ccount = 0;
        
        while(true)
        {
            //ID
            if(!getline(inp, tmpline))
                break;
            tempaccount.ID = tmpline; 
            
            //login
            if(!getline(inp, tmpline))
                break;
            tempaccount.login = tmpline;
                
            //pass
            if(!getline(inp, tmpline))
                break;
                
            tempaccount.password = tmpline;
                
            //days
            if(!getline(inp, tmpline))
                break;
            
            tempaccount.days_remaining = atoi(tmpline.c_str());
            
            //ban
            if(!getline(inp, tmpline))
                break;
            tempaccount.accountban = atoi(tmpline.c_str());
                
            //totalloads
            if(!getline(inp, tmpline))
                break;
            
            tempaccount.totalloads = atoi(tmpline.c_str());
            
            cachedAccounts.push_back(tempaccount);
        }
        inp.close();
    }
}

 void AFoxAccountsManager::FlushAccountsToDatabase()
 {
    std::ofstream out(FOX_SERVER_ACCOUNTSFILE, ofstream::out | ofstream::trunc);
    
    for(auto &a : cachedAccounts)
    {
        out << a.ID << endl;
        out << a.login << endl;
        out << a.password << endl;
        out << a.days_remaining << endl;
        out << a.accountban << endl;
        out << a.totalloads << endl;
    }
    
    out.close();
 }
 
 Account AFoxAccountsManager::getRaw(int ACCit)
 {
     if(ACCit < cachedAccounts.size())
        return cachedAccounts[ACCit];
    return Account("-1", "-1", "-1");
 }
 
void AFoxAccountsManager::DumpToLog()
{
    for(auto &a : cachedAccounts)
        FoxLogger->AppendInfo("\nL: %s\nP: %s\nID: %s\nDays: %u\nBanned: %d\nTotal loads: %u \n\n", a.login.c_str(), a.password.c_str(), a.ID.c_str(),
        a.days_remaining, a.accountban, a.totalloads);
}