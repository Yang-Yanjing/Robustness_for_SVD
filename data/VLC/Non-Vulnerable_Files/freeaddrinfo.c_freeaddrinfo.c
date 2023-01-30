 */
void freeaddrinfo (struct addrinfo *res)
{
    while (res != NULL)
    {
        struct addrinfo *next = res->ai_next;
        free (res->ai_canonname);
        free (res->ai_addr);
        free (res);
        res = next;
    }
}