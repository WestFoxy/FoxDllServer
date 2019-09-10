#pragma once
#include "includes.h"

using namespace std;

class AFoxLogger
{
    ofstream LogDescriptor;
    ofstream ConnectionsDescriptor;
    string TimeFormatted();
public:
    void AppendInfo(char *format, ...);
    void AppendError(char *format, ...);
    void AppendWarning(char *format, ...);
    
    void AppendConnection(string PCName, string ipaddress, string login, string password, string ID,bool success);
    
    AFoxLogger();
    ~AFoxLogger();
};

extern AFoxLogger *FoxLogger;