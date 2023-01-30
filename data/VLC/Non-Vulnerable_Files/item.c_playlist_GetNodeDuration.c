 */
mtime_t playlist_GetNodeDuration( playlist_item_t* node )
{
    /* For the assert */
    playlist_t *p_playlist = node->p_playlist;
    PL_ASSERT_LOCKED;
    mtime_t mt_duration = 0;
    if( node->i_children != -1 )
        for( int i = 0; i < node->i_children; i++ )
        {
            input_item_t* p_input = node->pp_children[i]->p_input;
            if ( p_input->i_type == ITEM_TYPE_NODE )
                mt_duration += playlist_GetNodeDuration( node->pp_children[i] );
            else
                mt_duration += input_item_GetDuration( p_input );
        }
    return mt_duration;
}
