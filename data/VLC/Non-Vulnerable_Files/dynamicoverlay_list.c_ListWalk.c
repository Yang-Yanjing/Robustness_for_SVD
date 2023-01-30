}
overlay_t *ListWalk( list_t *p_list )
{
    static overlay_t **pp_cur = NULL;
    if( pp_cur == NULL )
        pp_cur = p_list->pp_head;
    else
        pp_cur = pp_cur + 1;
    for( ; pp_cur < p_list->pp_tail; ++pp_cur )
    {
        if( ( *pp_cur != NULL ) &&
            ( (*pp_cur)->b_active )&&
            ( (*pp_cur)->format.i_chroma != VLC_FOURCC( '\0','\0','\0','\0') ) )
        {
            return *pp_cur;
        }
    }
    pp_cur = NULL;
    return NULL;
}
