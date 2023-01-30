}
static int dup_socket(int oldfd)
{
    int newfd;
#ifndef _WIN32
    newfd = vlc_dup(oldfd);
#else
    WSAPROTOCOL_INFO info;
    WSADuplicateSocket (oldfd, GetCurrentProcessId (), &info);
    newfd = WSASocket (info.iAddressFamily, info.iSocketType,
                       info.iProtocol, &info, 0, 0);
#endif
    return newfd;
}
