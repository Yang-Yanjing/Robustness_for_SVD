}
playlist_item_t * playlist_CurrentPlayingItem( playlist_t * p_playlist )
{
    PL_ASSERT_LOCKED;
    return pl_priv(p_playlist)->status.p_item;
}
