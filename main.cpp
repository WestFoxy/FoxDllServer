
#include "includes.h"
#include "Log.h"
#include "FoxConnect.h"
#include "AccountsManager.h"


const char *msg = "foxyfox";

int main(int argc, char **argv)
{
    try
    {
        
        FoxAccountsManager->LoadAccountsToCache();
        FoxAccountsManager->DumpToLog();
        
        FoxServer->StartServer();
    }
    catch(const std::runtime_error& e)
    {
        FoxLogger->AppendError("%s", (const char*)e.what());
        FoxLogger->AppendError("TRYING TO RESTART SERVER");
    }
    
}