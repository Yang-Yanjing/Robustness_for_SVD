};
static int st_Error (vlc_tls_t *obj, int val)
{
    switch (val)
    {
        case errSSLWouldBlock:
            errno = EAGAIN;
            break;
        case errSSLClosedGraceful:
        case errSSLClosedAbort:
            msg_Dbg(obj, "Connection closed with code %d", val);
            errno = ECONNRESET;
            break;
        default:
            msg_Err(obj, "Found error %d", val);
            errno = ECONNRESET;
    }
    return -1;
}
