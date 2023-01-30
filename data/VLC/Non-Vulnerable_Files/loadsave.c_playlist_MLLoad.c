}
int playlist_MLLoad( playlist_t *p_playlist )
{
    input_item_t *p_input;
    char *psz_datadir = config_GetUserDir( VLC_DATA_DIR );
    if( !psz_datadir ) /* XXX: This should never happen */
    {
        msg_Err( p_playlist, "no data directory, cannot load media library") ;
        return VLC_EGENERIC;
    }
    char *psz_file;
    if( asprintf( &psz_file, "%s" DIR_SEP "ml.xspf", psz_datadir ) == -1 )
        psz_file = NULL;
    free( psz_datadir );
    if( psz_file == NULL )
        return VLC_ENOMEM;
    /* lousy check for media library file */
    struct stat st;
    if( vlc_stat( psz_file, &st ) )
    {
        free( psz_file );
        return VLC_EGENERIC;
    }
    char *psz_uri = vlc_path2uri( psz_file, "file/xspf-open" );
    free( psz_file );
    if( psz_uri == NULL )
        return VLC_ENOMEM;
    const char *const options[1] = { "meta-file", };
    /* that option has to be cleaned in input_item_subitem_tree_added() */
    /* vlc_gc_decref() in the same function */
    p_input = input_item_NewExt( psz_uri, _("Media Library"),
                                 1, options, VLC_INPUT_OPTION_TRUSTED, -1 );
    free( psz_uri );
    if( p_input == NULL )
        return VLC_EGENERIC;
    PL_LOCK;
    if( p_playlist->p_media_library->p_input )
        vlc_gc_decref( p_playlist->p_media_library->p_input );
    p_playlist->p_media_library->p_input = p_input;
    vlc_event_attach( &p_input->event_manager, vlc_InputItemSubItemTreeAdded,
                        input_item_subitem_tree_added, p_playlist );
    pl_priv(p_playlist)->b_doing_ml = true;
    PL_UNLOCK;
    input_Read( p_playlist, p_input );
    PL_LOCK;
    pl_priv(p_playlist)->b_doing_ml = false;
    PL_UNLOCK;
    vlc_event_detach( &p_input->event_manager, vlc_InputItemSubItemTreeAdded,
                        input_item_subitem_tree_added, p_playlist );
    return VLC_SUCCESS;
}
