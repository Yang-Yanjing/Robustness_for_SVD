 */
playlist_item_t* playlist_ItemGetByInput( playlist_t * p_playlist,
                                          input_item_t *p_item )
{
    int i;
    PL_ASSERT_LOCKED;
    if( get_current_status_item( p_playlist ) &&
        get_current_status_item( p_playlist )->p_input == p_item )
    {
        return get_current_status_item( p_playlist );
    }
    /** \todo Check if this is always incremental and whether we can bsearch */
    for( i =  0 ; i < p_playlist->all_items.i_size; i++ )
    {
        if( ARRAY_VAL(p_playlist->all_items, i)->p_input == p_item )
        {
            return ARRAY_VAL(p_playlist->all_items, i);
        }
    }
    return NULL;
}
