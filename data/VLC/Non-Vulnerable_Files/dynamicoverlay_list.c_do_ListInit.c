 *****************************************************************************/
int do_ListInit( list_t *p_list )
{
    p_list->pp_head = calloc( 16, sizeof( overlay_t * ) );
    if( p_list->pp_head == NULL )
        return VLC_ENOMEM;
    p_list->pp_tail = p_list->pp_head + 16;
    return VLC_SUCCESS;
}
