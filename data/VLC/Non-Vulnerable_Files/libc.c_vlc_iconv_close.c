}
int vlc_iconv_close( vlc_iconv_t cd )
{
#ifndef __linux__
    if ( cd == (vlc_iconv_t)(-2) )
        return 0;
#endif
#if defined(HAVE_ICONV)
    return iconv_close( cd );
#else
    abort ();
#endif
}
