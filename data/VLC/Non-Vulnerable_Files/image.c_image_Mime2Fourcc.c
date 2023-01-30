};
vlc_fourcc_t image_Mime2Fourcc( const char *psz_mime )
{
    int i;
    for( i = 0; mime_table[i].i_codec; i++ )
        if( !strcmp( psz_mime, mime_table[i].psz_mime ) )
            return mime_table[i].i_codec;
    return 0;
}
