}
static int InstallFile( addons_storage_t *p_this, const char *psz_downloadlink,
                        const char *psz_dest )
{
    stream_t *p_stream;
    FILE *p_destfile;
    char buffer[1<<10];
    int i_read = 0;
    p_stream = stream_UrlNew( p_this, psz_downloadlink );
    if( !p_stream )
    {
        msg_Err( p_this, "Failed to access Addon download url %s", psz_downloadlink );
        return VLC_EGENERIC;
    }
    char *psz_path = strdup( psz_dest );
    if ( !psz_path )
    {
        stream_Delete( p_stream );
        return VLC_ENOMEM;
    }
    char *psz_buf = strrchr( psz_path, DIR_SEP_CHAR );
    if( psz_buf )
    {
        *++psz_buf = '\0';
        /* ensure directory exists */
        if( !EMPTY_STR( psz_path ) ) recursive_mkdir( VLC_OBJECT(p_this), psz_path );
        free( psz_path );
    }
    p_destfile = vlc_fopen( psz_dest, "w" );
    if( !p_destfile )
    {
        msg_Err( p_this, "Failed to open Addon storage file %s", psz_dest );
        stream_Delete( p_stream );
        return VLC_EGENERIC;
    }
    while ( ( i_read = stream_Read( p_stream, &buffer, 1<<10 ) ) )
    {
        if ( fwrite( &buffer, i_read, 1, p_destfile ) < 1 )
        {
            msg_Err( p_this, "Failed to write to Addon file" );
            fclose( p_destfile );
            stream_Delete( p_stream );
            return VLC_EGENERIC;
        }
    }
    fclose( p_destfile );
    stream_Delete( p_stream );
    return VLC_SUCCESS;
}
