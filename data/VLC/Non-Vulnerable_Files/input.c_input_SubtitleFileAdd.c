}
static void input_SubtitleFileAdd( input_thread_t *p_input, char *psz_subtitle,
                                   unsigned i_flags )
{
    /* if we are provided a subtitle.sub file,
     * see if we don't have a subtitle.idx and use it instead */
    char *psz_path = strdup( psz_subtitle );
    if( likely(psz_path != NULL) )
    {
        char *psz_extension = strrchr( psz_path, '.');
        if( psz_extension && strcmp( psz_extension, ".sub" ) == 0 )
        {
            struct stat st;
            strcpy( psz_extension, ".idx" );
            if( !vlc_stat( psz_path, &st ) && S_ISREG( st.st_mode ) )
            {
                msg_Dbg( p_input, "using %s as subtitle file instead of %s",
                         psz_path, psz_subtitle );
                strcpy( psz_subtitle, psz_path ); /* <- FIXME! constify */
            }
        }
        free( psz_path );
    }
    char *url = vlc_path2uri( psz_subtitle, NULL );
    if( url == NULL )
        return;
    input_SubtitleAdd( p_input, url, i_flags );
    free( url );
}
