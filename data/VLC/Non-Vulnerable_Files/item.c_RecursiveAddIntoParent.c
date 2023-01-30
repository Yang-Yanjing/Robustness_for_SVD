}
static int RecursiveAddIntoParent (
    playlist_t *p_playlist, playlist_item_t *p_parent,
    input_item_node_t *p_node, int i_pos, bool b_flat,
    playlist_item_t **pp_first_leaf )
{
    *pp_first_leaf = NULL;
    if( p_parent->i_children == -1 ) ChangeToNode( p_playlist, p_parent );
    if( i_pos == PLAYLIST_END ) i_pos = p_parent->i_children;
    for( int i = 0; i < p_node->i_children; i++ )
    {
        input_item_node_t *p_child_node = p_node->pp_children[i];
        playlist_item_t *p_new_item = NULL;
        bool b_children = p_child_node->i_children > 0;
        //Create the playlist item represented by input node, if allowed.
        if( !(b_flat && b_children) )
        {
            p_new_item = playlist_NodeAddInput( p_playlist,
                    p_child_node->p_item,
                    p_parent,
                    PLAYLIST_INSERT, i_pos,
                    pl_Locked );
            if( !p_new_item ) return i_pos;
            i_pos++;
        }
        //Recurse if any children
        if( b_children )
        {
            //Substitute p_new_item for first child leaf
            //(If flat, continue counting from current position)
            int i_last_pos = RecursiveAddIntoParent(
                    p_playlist,
                    p_new_item ? p_new_item : p_parent,
                    p_child_node,
                    ( b_flat ? i_pos : 0 ),
                    b_flat,
                    &p_new_item );
            //If flat, take position after recursion as current position
            if( b_flat ) i_pos = i_last_pos;
        }
        assert( p_new_item != NULL );
        if( i == 0 ) *pp_first_leaf = p_new_item;
    }
    return i_pos;
}
