# define vlc_pipe selectable_pipe
static int selectable_pipe (int fd[2])
{
    struct sockaddr_in addr;
    int addrlen = sizeof (addr);
    int l = socket (PF_INET, SOCK_STREAM, IPPROTO_TCP),
        c = socket (PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (l == -1 || c == -1)
        goto error;
    memset (&addr, 0, sizeof (addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl (INADDR_LOOPBACK);
    if (bind (l, (struct sockaddr *)&addr, sizeof (addr))
     || getsockname (l, (struct sockaddr *)&addr, &addrlen)
     || listen (l, 1)
     || connect (c, (struct sockaddr *)&addr, addrlen))
        goto error;
    int a = accept (l, NULL, NULL);
    if (a == -1)
        goto error;
    close (l);
    //shutdown (a, 0);
    //shutdown (c, 1);
    fd[0] = c;
    fd[1] = a;
    return 0;
error:
    if (l != -1)
        close (l);
    if (c != -1)
        close (c);
    return -1;
}
