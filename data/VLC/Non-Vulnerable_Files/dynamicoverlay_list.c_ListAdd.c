}
ssize_t ListAdd( list_t *p_list, overlay_t *p_new )
{
    /* Find an available slot */
    for( overlay_t **pp_cur = p_list->pp_head;
         pp_cur < p_list->pp_tail;
         ++pp_cur )
    {
        if( *pp_cur == NULL )
        {
            *pp_cur = p_new;
            return pp_cur - p_list->pp_head;
        }
    }
    /* Have to expand */
    size_t i_size = p_list->pp_tail - p_list->pp_head;
    size_t i_newsize = i_size * 2;
    p_list->pp_head = realloc_or_free( p_list->pp_head,
                                       i_newsize * sizeof( overlay_t * ) );
    if( p_list->pp_head == NULL )
        return VLC_ENOMEM;
    p_list->pp_tail = p_list->pp_head + i_newsize;
    memset( p_list->pp_head + i_size, 0, i_size * sizeof( overlay_t * ) );
    p_list->pp_head[i_size] = p_new;
    return i_size;
}
