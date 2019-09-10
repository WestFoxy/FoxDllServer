#include "FoxConnect.h"
#include "Log.h"
#include "AccountsManager.h"
#include "DLLManager.h"

AFoxServer *FoxServer = new AFoxServer;

using namespace std;

class ClientDataTransfer
{
private:
    unsigned char *allocated_input_data = 0;
    int *sock_client = 0;

public:
    void HandleError(int res, int clsz)
    {
        if (res == 0)
            throw(runtime_error("Nothing to get"));
        if (res == -1)
            throw(runtime_error("Client closed connect"));
        if (res < -1)
            throw(runtime_error("Unknown error HndErr()"));
        if (clsz > 4096 || clsz <= 0)
            throw(runtime_error("Outbound size of client message"));
    }
    void HandleError(int res)
    {
        if (res == 0)
            throw(runtime_error("Nothing to get"));
        if (res == -1)
            throw(runtime_error("Client closed connect"));
        if (res < -1)
            throw(runtime_error("Unknown error"));
    }
    int GetData()
    {
        int ssock = *this->sock_client;
        int client_data_size = 0;

        int res = recv(ssock, &client_data_size, 4, MSG_WAITALL);
        HandleError(res, client_data_size);
        
        res = recv(ssock, allocated_input_data, client_data_size, MSG_WAITALL);
        HandleError(res);
        
        return client_data_size;
    }
    unsigned char *GetRawData(int *client_size)
    {
        int res = GetData();
        if(client_size)
            *client_size = res;
        return this->allocated_input_data;
    }
    int GetClientCommand()
    {
        int commandsz = GetData();
        if(commandsz != 5)
            throw(runtime_error("command size != 5"));
        
        const char *dCommand = (const char*)this->allocated_input_data;
        
        if(strstr(dCommand, SERV_LOGIN_COMMAND)) //login
            return 0;
        if(strstr(dCommand, SERV_READYFORDOWNLOAD_COMMAND)) //Ready for download
            return 1;
        if(strstr(dCommand, SERV_LOGINADM_COMMAND))
            return 2;
        
    }
    void SendCommand(const char *comm)
    {
        SendData((void*)comm, 5);
    }
    void SendData(void *client_data, unsigned int data_size)
    {
        send(*this->sock_client, &data_size, 4, 0);
        send(*this->sock_client, client_data, data_size, 0);
    }
    void SetSocket(int sock)
    {
        *this->sock_client = sock;
        return;
    }

    ClientDataTransfer()
    {
        sock_client = new int;
        
        if (!sock_client)
            throw(runtime_error("mem sock null"));

        allocated_input_data = new unsigned char[524288];
        if (!allocated_input_data)
            throw(runtime_error("mem inpdata null"));
        
        
    }
    ~ClientDataTransfer()
    {
        //FoxLogger->AppendInfo("DELETING INT: 0x%x INP_DATA: 0x%x", sock_client, allocated_input_data);
        delete[] allocated_input_data;
        delete sock_client;
    }
};

void *HandleClient(void *client_desc)
{
    pthread_detach(pthread_self());
    int sock = *(int*)client_desc;
    
    ClientDataTransfer *dtTransfer = 0;
    try
    {
        dtTransfer = new ClientDataTransfer;
        if(!dtTransfer)
            throw(runtime_error("ClientDataTransfer err alloc"));
        dtTransfer->SetSocket(sock);
        
        int command = dtTransfer->GetClientCommand();
        if(command == 0)
        {
            dtTransfer->SendCommand(SERV_LOGIN_OKAY);
            const char *LoginDetailsRaw = (const char*)dtTransfer->GetRawData(0);
            
            auto it = FoxAccountsManager->GetIteratorOfAccount(LoginDetailsRaw);
            if(it == -1)
            {
                dtTransfer->SendCommand(SERV_LOGIN_INVALID_PASS);
                throw(runtime_error("LoginPass invalid"));
            }
            if(it == -2)
            {
                dtTransfer->SendCommand(SERV_LOGIN_INVALID_HWID);
                throw(runtime_error("HWID invalid"));
            }
            if(it == -3)
            {
                dtTransfer->SendCommand(SERV_LOGIN_INVALID_PARSE);
                throw(runtime_error("Parse failed"));
            }
            dtTransfer->SendCommand(SERV_LOGINVALID);
            command = dtTransfer->GetClientCommand();
            if(command == 1) //READY FOR DOWNLOAD
            {
                dtTransfer->SendData(&FoxDll->crc32sum, 4); //crc32 sum
                dtTransfer->SendData(FoxDll->DLLData, FoxDll->DLLData_Size);
                FoxLogger->AppendInfo("Sent dll. crc: %x", FoxDll->crc32sum);
            }
        }
        else if(command == 2) //ADMIN LOGIN
        {
            dtTransfer->SendCommand(SERV_LOGIN_OKAY);
            const char *LoginDetailsRaw = (const char*)dtTransfer->GetRawData(0);
            
            auto result = FoxAccountsManager->GetAdminPanel(LoginDetailsRaw);
            if(result != 666)
            {
                dtTransfer->SendCommand(SERV_LOGIN_INVALID_PASS);
                goto end;
                
            }
            dtTransfer->SendCommand(SERV_LOGINVALID);
            /*while(true)
            {
                dtTransfer->GetClientCommand();
            }*/
        }
    }
    catch(const std::runtime_error& e)
    {
        FoxLogger->AppendError("%s [%d]", e.what(), sock);
        //dtTransfer->~ClientDataTransfer();
        
        delete dtTransfer;
        close(sock);
         pthread_exit(nullptr);
        return 0;
    }
    end:
    //dtTransfer->~ClientDataTransfer();
    delete dtTransfer;
    close(sock);
    pthread_exit(nullptr);
    
    return 0;
}

void AFoxServer::StartServer(int port)
{
    int temp_client_desc = 0, c;
    
    serversock_descriptor = socket(AF_INET , SOCK_STREAM , 0);
    if(serversock_descriptor == -1)
        throw(runtime_error("Could not create socket"));
    FoxLogger->AppendInfo("FoxServerSocket created. Descriptor = %d", serversock_descriptor);
    
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons( FOX_SERVER_PORT );
    
    if( bind(serversock_descriptor,(struct sockaddr *)&server , sizeof(server)) < 0)
        throw(runtime_error("Bind socket failed"));
    FoxLogger->AppendInfo("FoxServerSocket binded. Server port = %d", FOX_SERVER_PORT);
    
    listen(serversock_descriptor, 3);
    
    pthread_t thread_id;
    char ipaddress[INET_ADDRSTRLEN];
    
    c = sizeof(struct sockaddr_in);
    while( (temp_client_desc = accept(serversock_descriptor, (struct sockaddr *)&client, (socklen_t*)&c)) )
    {
        
        inet_ntop( AF_INET, &client.sin_addr, ipaddress, INET_ADDRSTRLEN );
        
        FoxLogger->AppendInfo("Connection from %s. Sock id=%d", ipaddress, temp_client_desc);
        
        if( pthread_create( &thread_id , NULL ,  HandleClient , (void*) &temp_client_desc) < 0)
            throw runtime_error("Could not create thread");
        
        //FoxLogger->AppendInfo("Thread created. pthread id=%lu",thread_id);
    }
    
    return;
}