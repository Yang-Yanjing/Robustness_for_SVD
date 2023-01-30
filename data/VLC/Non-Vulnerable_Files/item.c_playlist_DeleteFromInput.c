 */
int playlist_DeleteFromInput( playlist_t *p_playlist, input_item_t *p_input,
                              bool b_locked )
{
    int i_ret;
    PL_LOCK_IF( !b_locked );
    i_ret = DeleteFromInput( p_playlist, p_input,
                             p_playlist->p_root, true );
    PL_UNLOCK_IF( !b_locked );
    return ( i_ret == VLC_SUCCESS ? VLC_SUCCESS : VLC_ENOITEM );
}
