#ifdef _WIN32
static int vlc_socket_pair( int fds[2] )
{
    struct sockaddr_in inaddr;
    struct sockaddr addr;
    SOCKET lst = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );
    memset( &inaddr, 0, sizeof( inaddr ) );
    memset( &addr, 0, sizeof( addr ) );
    inaddr.sin_family = AF_INET;
    inaddr.sin_addr.s_addr = htonl( INADDR_LOOPBACK );
    inaddr.sin_port = 0;
    int yes = 1;
    setsockopt( lst, SOL_SOCKET, SO_REUSEADDR, (char*)&yes, sizeof( yes ) );
    bind( lst, (struct sockaddr *)&inaddr, sizeof( inaddr ) );
    listen( lst, 1 );
    int len = sizeof( inaddr );
    getsockname( lst, &addr, &len );
    fds[0] = socket( AF_INET, SOCK_STREAM, 0 );
    connect( fds[0], &addr, len );
    fds[1] = accept( lst, 0, 0 );
    closesocket( lst );
    return 0;
}
