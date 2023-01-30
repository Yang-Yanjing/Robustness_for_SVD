 *****************************************************************************/
vlc_iconv_t vlc_iconv_open( const char *tocode, const char *fromcode )
{
#ifndef __linux__
    if( !strcasecmp(tocode, "UTF-8") && !strcasecmp(fromcode, "ISO_6937") )
        return (vlc_iconv_t)(-2);
#endif
#if defined(HAVE_ICONV)
# if defined(__OS2__) && defined(__INNOTEK_LIBC__)
    char tocode_ucs2[] = "UCS-2LE";
    char fromcode_ucs2[] = "UCS-2LE";
    os2_iconv_t *p_os2_iconv;
    /* Workaround for UTF-16 because OS/2 supports UCS-2 only not UTF-16 */
    if( !strncmp( tocode, "UTF-16", 6 ))
    {
        strncpy( tocode_ucs2 + 5, tocode + 6, 2 );
        tocode = tocode_ucs2;
    }
    if( !strncmp( fromcode, "UTF-16", 6 ))
    {
        strncpy( fromcode_ucs2 + 5, fromcode + 6, 2 );
        fromcode = fromcode_ucs2;
    }
    p_os2_iconv = ( os2_iconv_t * )iconv_open( tocode, fromcode );
    if( p_os2_iconv != ( iconv_t )(-1))
    {
        /* Mimic a behavior of GNU libiconv */
        uconv_attribute_t attr;
        UniQueryUconvObject( p_os2_iconv->from, &attr,
                             sizeof( uconv_attribute_t ),
                             NULL, NULL, NULL );
        attr.converttype |= CVTTYPE_PATH;
        UniSetUconvObject( p_os2_iconv->from, &attr );
    }
    return ( vlc_iconv_t )p_os2_iconv;
# else
    return iconv_open( tocode, fromcode );
# endif
#else
    return (vlc_iconv_t)(-1);
#endif
}
