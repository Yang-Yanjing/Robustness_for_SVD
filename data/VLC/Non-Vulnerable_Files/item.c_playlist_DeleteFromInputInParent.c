 */
int playlist_DeleteFromInputInParent( playlist_t *p_playlist,
                                      input_item_t *p_item,
                                      playlist_item_t *p_root, bool b_locked )
{
    int i_ret;
    PL_LOCK_IF( !b_locked );
    i_ret = DeleteFromInput( p_playlist, p_item, p_root, true );
    PL_UNLOCK_IF( !b_locked );
    return i_ret;
}
