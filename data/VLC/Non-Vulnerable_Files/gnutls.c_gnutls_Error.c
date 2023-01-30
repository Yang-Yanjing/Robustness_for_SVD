#endif
static int gnutls_Error (vlc_object_t *obj, int val)
{
    switch (val)
    {
        case GNUTLS_E_AGAIN:
#ifdef _WIN32
            WSASetLastError (WSAEWOULDBLOCK);
#else
            errno = EAGAIN;
#endif
            break;
        case GNUTLS_E_INTERRUPTED:
#ifdef _WIN32
            WSASetLastError (WSAEINTR);
#else
            errno = EINTR;
#endif
            break;
        default:
            msg_Err (obj, "%s", gnutls_strerror (val));
#ifndef NDEBUG
            if (!gnutls_error_is_fatal (val))
                msg_Err (obj, "Error above should be handled");
#endif
#ifdef _WIN32
            WSASetLastError (WSAECONNRESET);
#else
            errno = ECONNRESET;
#endif
    }
    return -1;
}
