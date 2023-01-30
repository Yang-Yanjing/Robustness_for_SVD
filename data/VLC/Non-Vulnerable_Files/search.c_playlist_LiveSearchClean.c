 */
static void playlist_LiveSearchClean( playlist_item_t *p_root )
{
    for( int i = 0; i < p_root->i_children; i++ )
    {
        playlist_item_t *p_item = p_root->pp_children[i];
        if( p_item->i_children >= 0 )
            playlist_LiveSearchClean( p_item );
        p_item->i_flags &= ~PLAYLIST_DBL_FLAG;
    }
}
