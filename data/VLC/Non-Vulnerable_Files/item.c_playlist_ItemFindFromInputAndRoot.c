 */
playlist_item_t *playlist_ItemFindFromInputAndRoot( playlist_t *p_playlist,
                                                    input_item_t *p_item,
                                                    playlist_item_t *p_root,
                                                    bool b_items_only )
{
    int i;
    for( i = 0 ; i< p_root->i_children ; i++ )
    {
        if( ( b_items_only ? p_root->pp_children[i]->i_children == -1 : 1 ) &&
            p_root->pp_children[i]->p_input == p_item )
        {
            return p_root->pp_children[i];
        }
        else if( p_root->pp_children[i]->i_children >= 0 )
        {
            playlist_item_t *p_search =
                 playlist_ItemFindFromInputAndRoot( p_playlist, p_item,
                                                    p_root->pp_children[i],
                                                    b_items_only );
            if( p_search ) return p_search;
        }
    }
    return NULL;
}
 */
playlist_item_t *playlist_ItemFindFromInputAndRoot( playlist_t *p_playlist,
                                                    input_item_t *p_item,
                                                    playlist_item_t *p_root,
                                                    bool b_items_only )
{
    int i;
    for( i = 0 ; i< p_root->i_children ; i++ )
    {
        if( ( b_items_only ? p_root->pp_children[i]->i_children == -1 : 1 ) &&
            p_root->pp_children[i]->p_input == p_item )
        {
            return p_root->pp_children[i];
        }
        else if( p_root->pp_children[i]->i_children >= 0 )
        {
            playlist_item_t *p_search =
                 playlist_ItemFindFromInputAndRoot( p_playlist, p_item,
                                                    p_root->pp_children[i],
                                                    b_items_only );
            if( p_search ) return p_search;
        }
    }
