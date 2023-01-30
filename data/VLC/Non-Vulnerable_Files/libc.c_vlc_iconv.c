}
size_t vlc_iconv( vlc_iconv_t cd, const char **inbuf, size_t *inbytesleft,
                  char **outbuf, size_t *outbytesleft )
{
#ifndef __linux__
    if ( cd == (vlc_iconv_t)(-2) )
        return ISO6937toUTF8( (const unsigned char **)inbuf, inbytesleft,
                              (unsigned char **)outbuf, outbytesleft );
#endif
#if defined(HAVE_ICONV)
    return iconv( cd, (ICONV_CONST char **)inbuf, inbytesleft,
                  outbuf, outbytesleft );
#else
    abort ();
#endif
}
