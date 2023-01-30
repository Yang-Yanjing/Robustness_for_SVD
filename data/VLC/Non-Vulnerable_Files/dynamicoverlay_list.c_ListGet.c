}
overlay_t *ListGet( list_t *p_list, size_t i_idx )
{
    if( ( i_idx >= (size_t)( p_list->pp_tail - p_list->pp_head ) ) ||
        ( p_list->pp_head[i_idx] == NULL ) )
    {
        return NULL;
    }
    return p_list->pp_head[i_idx];
}
