 */
static int recursiveNodeSort( playlist_t *p_playlist, playlist_item_t *p_node,
                              sortfn_t p_sortfn )
{
    int i;
    playlist_ItemArraySort(p_node->i_children,p_node->pp_children,p_sortfn);
    for( i = 0 ; i< p_node->i_children; i++ )
    {
        if( p_node->pp_children[i]->i_children != -1 )
        {
            recursiveNodeSort( p_playlist, p_node->pp_children[i], p_sortfn );
        }
    }
    return VLC_SUCCESS;
}
