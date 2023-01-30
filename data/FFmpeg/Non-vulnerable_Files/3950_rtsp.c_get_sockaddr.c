static int get_sockaddr(const char *buf, struct sockaddr_storage *sock)
{
    struct addrinfo hints = { 0 }, *ai = NULL;
    hints.ai_flags = AI_NUMERICHOST;
    if (getaddrinfo(buf, NULL, &hints, &ai))
        return -1;
    memcpy(sock, ai->ai_addr, FFMIN(sizeof(*sock), ai->ai_addrlen));
    freeaddrinfo(ai);
    return 0;
}
