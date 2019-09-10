#include "Log.h"
#define FOX_DEBUG

AFoxLogger *FoxLogger = new AFoxLogger;
string AFoxLogger::TimeFormatted()
{
    string res;
    auto curtime = time(nullptr);
    res = std::asctime(localtime(&curtime));
    res.erase(res.end()-1);

    return res;
}

void AFoxLogger::AppendInfo(char *format, ...)
{
    char buff[1024];
    
    va_list args;

    va_start(args, format);
    vsprintf(buff,format, args);
    va_end(args);
    
    
    LogDescriptor << "[INF][" << TimeFormatted() << "]: " << buff << endl;
    
    #ifdef FOX_DEBUG
    cout << "[INF][" << TimeFormatted() << "]: " << buff << endl;
    #endif
}

void AFoxLogger::AppendError(char *format, ...)
{
    char buff[1024];
    
    va_list args;

    va_start(args, format);
    vsprintf(buff,format, args);
    va_end(args);
    
    LogDescriptor << "[ERR][" << TimeFormatted() << "]: " << buff << endl;
    #ifdef FOX_DEBUG
    cout << "[ERR][" << TimeFormatted() << "]: " << buff << endl;
    #endif
}
void AFoxLogger::AppendWarning(char *format, ...)
{
    
    char buff[1024];
    
    va_list args;

    va_start(args, format);
    vsprintf(buff,format, args);
    va_end(args);
    
    LogDescriptor << "[WARN][" << TimeFormatted() << "]: " << buff << endl;
    
    #ifdef FOX_DEBUG
    cout << "[WARN][" << TimeFormatted() << "]: " << buff << endl;
    #endif
}

void AFoxLogger::AppendConnection(string PCName, string ipaddress, string login, string password, string ID, bool success)
{
    ConnectionsDescriptor << "[" << TimeFormatted() << (success ? "]\nPCName: " : "] ERROR\nPCName: ") << PCName
    << "\nL: " << login << "\nP: " << password << "\nID: " << ID << endl << endl << endl;
    
    #ifdef FOX_DEBUG
    cout << "[" << TimeFormatted() << (success ? "]\nPCName: " : "] ERROR\nPCName: ") << PCName
    << "\nL: " << login << "\nP: " << password << "\nID: " << ID << endl << endl << endl;
    #endif
}

AFoxLogger::AFoxLogger()
{
    this->ConnectionsDescriptor.open("LOGLISAConnections.log");
    this->LogDescriptor.open("LOGLISA.log");
    if(!ConnectionsDescriptor.is_open() || !LogDescriptor.is_open())
        throw(runtime_error("FoxLogFile open error"));
        
    return;
}
AFoxLogger::~AFoxLogger()
{
    this->ConnectionsDescriptor.close();
    this->LogDescriptor.close();
}