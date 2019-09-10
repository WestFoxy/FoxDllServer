#include "DLLManager.h"
#include "Log.h"
#include "includes.h"


using namespace std;

AFoxDll *FoxDll = new AFoxDll;

#define POLY 0x82f63b78

/* CRC-32 (Ethernet, ZIP, etc.) polynomial in reversed bit order. */
/* #define POLY 0xedb88320 */

uint32_t crc32c(uint32_t crc, const unsigned char *buf, size_t len)
{
    int k;

    crc = ~crc;
    while (len--) {
        crc ^= *buf++;
        for (k = 0; k < 8; k++)
            crc = crc & 1 ? (crc >> 1) ^ POLY : crc >> 1;
    }
    return ~crc;
}

void AFoxDll::ReloadDllToBuffer()
{
    //FoxLogger->AppendInfo("Reloading dll");
    ifstream DllDescriptor("LISA.dll", std::ios::binary);
    if(!DllDescriptor.is_open())
        throw(runtime_error("could not open LISA.dll"));
    
    //Get size
    DllDescriptor.seekg(0, ios::end);
    this->DLLData_Size = DllDescriptor.tellg();
    DllDescriptor.seekg(0, ios::beg);
    
    if(this->DLLData_Size > MAX_FOXSERVER_FILESIZE)
    {
        DllDescriptor.close();
        throw(runtime_error("DLLData_Size > MAX_FOXSERVER_FILESIZE"));
    }
    
    //read to buf
    DllDescriptor.read((char*)this->DLLData, this->DLLData_Size);
    DllDescriptor.close();
    
    
    this->crc32sum = crc32c(0, DLLData, DLLData_Size);
    
    
    //FoxLogger->AppendInfo("Reloaded DLL. File size = %d", this->DLLData_Size);
}

AFoxDll::AFoxDll()
{
    ReloadDllToBuffer();
}
AFoxDll::~AFoxDll()
{
    
}