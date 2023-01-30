 */
playlist_item_t* playlist_ItemGetById( playlist_t * p_playlist , int i_id )
{
    int i;
    PL_ASSERT_LOCKED;
    ARRAY_BSEARCH( p_playlist->all_items,->i_id, int, i_id, i );
    if( i != -1 )
        return ARRAY_VAL( p_playlist->all_items, i );
    else
        return NULL;
}
