}
int playlist_MLDump( playlist_t *p_playlist )
{
    char *psz_temp;
    psz_temp = config_GetUserDir( VLC_DATA_DIR );
    if( !psz_temp ) /* XXX: This should never happen */
    {
        msg_Err( p_playlist, "no data directory, cannot save media library") ;
        return VLC_EGENERIC;
    }
    char psz_dirname[ strlen( psz_temp ) + sizeof( DIR_SEP "ml.xspf")];
    strcpy( psz_dirname, psz_temp );
    free( psz_temp );
    if( config_CreateDir( (vlc_object_t *)p_playlist, psz_dirname ) )
    {
        return VLC_EGENERIC;
    }
    strcat( psz_dirname, DIR_SEP "ml.xspf" );
    if ( asprintf( &psz_temp, "%s.tmp%"PRIu32, psz_dirname, (uint32_t)getpid() ) < 1 )
        return VLC_EGENERIC;
    int i_ret = playlist_Export( p_playlist, psz_temp, p_playlist->p_media_library,
                     "export-xspf" );
    if ( i_ret != VLC_SUCCESS )
    {
        vlc_unlink( psz_temp );
        free( psz_temp );
        return i_ret;
    }
    i_ret = vlc_rename( psz_temp, psz_dirname );
    free( psz_temp );
    if( i_ret == -1 )
    {
        msg_Err( p_playlist, "could not rename %s.tmp: %s",
                 psz_dirname, vlc_strerror_c(errno) );
        return VLC_EGENERIC;
    }
    return VLC_SUCCESS;
}
