 */
playlist_item_t * get_current_status_item( playlist_t * p_playlist )
{
    PL_ASSERT_LOCKED;
    return pl_priv(p_playlist)->status.p_item;
}
