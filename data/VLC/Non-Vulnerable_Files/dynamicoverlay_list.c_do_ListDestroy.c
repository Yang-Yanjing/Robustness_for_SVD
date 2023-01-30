}
int do_ListDestroy( list_t *p_list )
{
    for( overlay_t **pp_cur = p_list->pp_head;
         pp_cur < p_list->pp_tail;
         ++pp_cur )
    {
        if( *pp_cur != NULL )
        {
            OverlayDestroy( *pp_cur );
            free( *pp_cur );
        }
    }
    free( p_list->pp_head );
    return VLC_SUCCESS;
}
