};
const char *vlc_strerror_c(int errnum)
{
    /* C run-time errors */
    if ((unsigned)errnum < (unsigned)_sys_nerr)
        return _sys_errlist[errnum];
    /* Windows socket errors */
    for (const wsaerrmsg_t *e = wsaerrmsg; e->msg != NULL; e++)
        if (e->code == errnum)
            return e->msg;
    return "Unknown error";
}
