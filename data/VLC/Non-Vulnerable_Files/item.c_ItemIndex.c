}
static int ItemIndex ( playlist_item_t *p_item )
{
    for( int i = 0; i < p_item->p_parent->i_children; i++ )
        if( p_item->p_parent->pp_children[i] == p_item ) return i;
    return -1;
}
