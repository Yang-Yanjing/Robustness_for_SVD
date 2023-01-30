*/
int playlist_AddExt( playlist_t *p_playlist, const char * psz_uri,
                     const char *psz_name, int i_mode, int i_pos,
                     mtime_t i_duration,
                     int i_options, const char *const *ppsz_options,
                     unsigned i_option_flags,
                     bool b_playlist, bool b_locked )
{
    int i_ret;
    input_item_t *p_input;
    p_input = input_item_NewExt( psz_uri, psz_name,
                                 i_options, ppsz_options, i_option_flags,
                                 i_duration );
    if( p_input == NULL )
        return VLC_ENOMEM;
    i_ret = playlist_AddInput( p_playlist, p_input, i_mode, i_pos, b_playlist,
                               b_locked );
    vlc_gc_decref( p_input );
    return i_ret;
}
