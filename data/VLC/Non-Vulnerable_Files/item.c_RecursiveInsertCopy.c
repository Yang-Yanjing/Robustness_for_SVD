}
static int RecursiveInsertCopy (
    playlist_t *p_playlist, playlist_item_t *p_item,
    playlist_item_t *p_parent, int i_pos, bool b_flat )
{
    PL_ASSERT_LOCKED;
    assert( p_parent != NULL && p_item != NULL );
    if( p_item == p_parent ) return i_pos;
    input_item_t *p_input = p_item->p_input;
    if( !(p_item->i_children != -1 && b_flat) )
    {
        input_item_t *p_new_input = input_item_Copy( p_input );
        if( !p_new_input ) return i_pos;
        playlist_item_t *p_new_item = NULL;
        if( p_item->i_children == -1 )
            p_new_item = playlist_NodeAddInput( p_playlist, p_new_input,
                                   p_parent, PLAYLIST_INSERT, i_pos,
                                   pl_Locked );
        else
            p_new_item = playlist_NodeCreate( p_playlist, NULL,
                                 p_parent, i_pos, 0, p_new_input );
        vlc_gc_decref( p_new_input );
        if( !p_new_item ) return i_pos;
        i_pos++;
        if( p_new_item->i_children != -1 )
            p_parent = p_new_item;
    }
    for( int i = 0; i < p_item->i_children; i++ )
    {
        if( b_flat )
            i_pos = RecursiveInsertCopy( p_playlist, p_item->pp_children[i],
                                         p_parent, i_pos, true );
        else
            RecursiveInsertCopy( p_playlist, p_item->pp_children[i],
                                 p_parent, p_parent->i_children, false );
    }
    return i_pos;
}
