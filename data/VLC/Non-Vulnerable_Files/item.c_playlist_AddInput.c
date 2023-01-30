*/
int playlist_AddInput( playlist_t* p_playlist, input_item_t *p_input,
                       int i_mode, int i_pos, bool b_playlist,
                       bool b_locked )
{
    playlist_item_t *p_item;
    if( !pl_priv(p_playlist)->b_doing_ml )
        PL_DEBUG( "adding item `%s' ( %s )", p_input->psz_name,
                                             p_input->psz_uri );
    PL_LOCK_IF( !b_locked );
    p_item = playlist_ItemNewFromInput( p_playlist, p_input );
    if( p_item == NULL )
    {
        PL_UNLOCK_IF( !b_locked );
        return VLC_ENOMEM;
    }
    AddItem( p_playlist, p_item,
             b_playlist ? p_playlist->p_playing :
                          p_playlist->p_media_library , i_mode, i_pos );
    GoAndPreparse( p_playlist, i_mode, p_item );
    PL_UNLOCK_IF( !b_locked );
    return VLC_SUCCESS;
}
