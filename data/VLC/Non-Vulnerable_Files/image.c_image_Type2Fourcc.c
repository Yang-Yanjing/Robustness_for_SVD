};
vlc_fourcc_t image_Type2Fourcc( const char *psz_type )
{
    for( unsigned i = 0; i < ARRAY_SIZE(ext_table); i++ )
        if( !strcasecmp( ext_table[i].psz_ext, psz_type ) )
            return ext_table[i].i_codec;
    return 0;
}
