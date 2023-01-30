}
int ListRemove( list_t *p_list, size_t i_idx )
{
    int ret;
    if( ( i_idx >= (size_t)( p_list->pp_tail - p_list->pp_head ) ) ||
        ( p_list->pp_head[i_idx] == NULL ) )
    {
        return VLC_EGENERIC;
    }
    ret = OverlayDestroy( p_list->pp_head[i_idx] );
    free( p_list->pp_head[i_idx] );
    p_list->pp_head[i_idx] = NULL;
    return ret;
}
