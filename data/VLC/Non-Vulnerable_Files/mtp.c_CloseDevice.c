}
static void CloseDevice( services_discovery_t *p_sd )
{
    input_item_t **pp_items = p_sd->p_sys->pp_items;
    if( pp_items != NULL )
    {
        for( int i_i = 0; i_i < p_sd->p_sys->i_count; i_i++ )
        {
            if( pp_items[i_i] != NULL )
            {
                services_discovery_RemoveItem( p_sd, pp_items[i_i] );
                vlc_gc_decref( pp_items[i_i] );
            }
        }
        free( pp_items );
    }
}
