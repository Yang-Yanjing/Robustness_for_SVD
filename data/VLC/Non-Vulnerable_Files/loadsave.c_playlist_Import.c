}
int playlist_Import( playlist_t *p_playlist, const char *psz_file )
{
    input_item_t *p_input;
    const char *const psz_option = "meta-file";
    char *psz_uri = vlc_path2uri( psz_file, NULL );
    if( psz_uri == NULL )
        return VLC_EGENERIC;
    p_input = input_item_NewExt( psz_uri, psz_file,
                                 1, &psz_option, VLC_INPUT_OPTION_TRUSTED, -1 );
    free( psz_uri );
    playlist_AddInput( p_playlist, p_input, PLAYLIST_APPEND, PLAYLIST_END,
                       true, false );
    return input_Read( p_playlist, p_input );
}
