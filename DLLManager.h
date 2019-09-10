#pragma once
#define MAX_FOXSERVER_FILESIZE  15728640

#include <stddef.h>
#include <stdint.h>

using namespace std;

class AFoxDll
{
public:
    unsigned char DLLData[MAX_FOXSERVER_FILESIZE];
    unsigned int DLLData_Size = 0;
    
    uint32_t crc32sum = 0;
    
    void ReloadDllToBuffer();
    AFoxDll();
    ~AFoxDll();
};

extern AFoxDll *FoxDll;