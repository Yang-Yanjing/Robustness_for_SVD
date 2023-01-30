DWORD LoadLibraryFlags = 0;
static int system_InitWSA(int hi, int lo)
{
    WSADATA data;
    if (WSAStartup(MAKEWORD(hi, lo), &data) == 0)
    {
        if (LOBYTE(data.wVersion) == 2 && HIBYTE(data.wVersion) == 2)
            return 0;
        /* Winsock DLL is not usable */
        WSACleanup( );
    }
    return -1;
}
