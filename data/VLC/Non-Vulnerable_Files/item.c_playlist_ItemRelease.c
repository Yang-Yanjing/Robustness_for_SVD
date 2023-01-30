*/
int playlist_ItemRelease( playlist_item_t *p_item )
{
    /* For the assert */
    playlist_t *p_playlist = p_item->p_playlist;
    PL_ASSERT_LOCKED;
    /* Surprise, we can't actually do more because we
     * don't do refcounting, or eauivalent.
     * Because item are not only accessed by their id
     * using playlist_item outside the PL_LOCK isn't safe.
     * Most of the modules does that.
     *
     * Who wants to add proper memory management? */
    uninstall_input_item_observer( p_item );
    ARRAY_APPEND( pl_priv(p_playlist)->items_to_delete, p_item);
    return VLC_SUCCESS;
}
