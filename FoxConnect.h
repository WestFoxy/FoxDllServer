#pragma once
#include "includes.h"

#define SERV_LOGIN_COMMAND              "L000"
#define SERV_LOGINADM_COMMAND           "L001"
#define SERV_LOGIN_INVALID_PASS         "LI00"
#define SERV_LOGIN_INVALID_HWID         "LI01"
#define SERV_LOGIN_INVALID_BANNED       "LI02"
#define SERV_LOGIN_INVALID_PARSE        "LI03"
#define SERV_LOGIN_OKAY                 "LO00"
#define SERV_LOGINVALID                 "LV00"
#define SERV_READYFORDATA               "R000"
#define SERV_READYFORDOWNLOAD_COMMAND   "R001"

#define FOX_SERVER_PORT 13251

class AFoxServer
{
private:
    int serversock_descriptor = 0;
    unsigned char NumClients = 0; //255 max
    int clientssock_descriptors[256] = {0};
    
    struct sockaddr_in server , client;
public:
    void StartServer(int port = 25312);
};

extern AFoxServer *FoxServer;