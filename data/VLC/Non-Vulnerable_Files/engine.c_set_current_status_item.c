}
void set_current_status_item( playlist_t * p_playlist,
    playlist_item_t * p_item )
{
    PL_ASSERT_LOCKED;
    if( pl_priv(p_playlist)->status.p_item &&
        pl_priv(p_playlist)->status.p_item->i_flags & PLAYLIST_REMOVE_FLAG &&
        pl_priv(p_playlist)->status.p_item != p_item )
    {
        /* It's unsafe given current design to delete a playlist item :(
        playlist_ItemDelete( pl_priv(p_playlist)->status.p_item ); */
    }
    pl_priv(p_playlist)->status.p_item = p_item;
}
