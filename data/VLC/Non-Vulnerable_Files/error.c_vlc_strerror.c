}
const char *vlc_strerror(int errnum)
{
    return /*vlc_gettext*/(vlc_strerror_c(errnum));
}
