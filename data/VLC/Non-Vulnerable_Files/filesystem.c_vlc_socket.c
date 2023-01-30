#include <vlc_network.h>
int vlc_socket (int pf, int type, int proto, bool nonblock)
{
    int fd = socket (pf, type, proto);
    if (fd == -1)
        return -1;
    if (nonblock)
        ioctlsocket (fd, FIONBIO, &(unsigned long){ 1 });
    return fd;
}
