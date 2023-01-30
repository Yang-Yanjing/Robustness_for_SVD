 */
static int getInputPosition( playlist_t* p_playlist, int i_input_id )
{
    input_item_t* p_input = NULL;
    assert( p_playlist );
    assert( i_input_id >= 0 );
    playlist_AssertLocked( p_playlist );
    for( int i = 0; i < playlist_CurrentSize( p_playlist ); i++ )
    {
        p_input = p_playlist->current.p_elems[i]->p_input;
        if( !p_input )
            return VLC_EGENERIC;
        if( p_input->i_id == i_input_id )
            return i;
    }
    return VLC_ENOITEM;
}
