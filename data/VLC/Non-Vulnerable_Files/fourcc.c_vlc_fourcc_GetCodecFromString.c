}
vlc_fourcc_t vlc_fourcc_GetCodecFromString( int i_cat, const char *psz_fourcc )
{
    if( !psz_fourcc || strlen(psz_fourcc) != 4 )
        return 0;
    return vlc_fourcc_GetCodec( i_cat,
                                VLC_FOURCC( psz_fourcc[0], psz_fourcc[1],
                                            psz_fourcc[2], psz_fourcc[3] ) );
}
