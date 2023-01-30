}
static vlc_list_t * NewList( int i_count )
{
    vlc_list_t * p_list = malloc( sizeof( vlc_list_t ) );
    if( p_list == NULL )
        return NULL;
    p_list->i_count = i_count;
    if( i_count == 0 )
    {
        p_list->p_values = NULL;
        return p_list;
    }
    p_list->p_values = malloc( i_count * sizeof( vlc_value_t ) );
    if( p_list->p_values == NULL )
    {
        p_list->i_count = 0;
        return p_list;
    }
    return p_list;
}
