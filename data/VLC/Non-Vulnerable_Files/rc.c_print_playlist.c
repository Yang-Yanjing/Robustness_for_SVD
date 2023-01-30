}
static void print_playlist( intf_thread_t *p_intf, playlist_item_t *p_item, int i_level )
{
    int i;
    char psz_buffer[MSTRTIME_MAX_SIZE];
    for( i = 0; i< p_item->i_children; i++ )
    {
        if( p_item->pp_children[i]->p_input->i_duration != -1 )
        {
            secstotimestr( psz_buffer, p_item->pp_children[i]->p_input->i_duration / 1000000 );
            msg_rc( "|%*s- %s (%s)", 2 * i_level, "", p_item->pp_children[i]->p_input->psz_name, psz_buffer );
        }
        else
            msg_rc( "|%*s- %s", 2 * i_level, "", p_item->pp_children[i]->p_input->psz_name );
        if( p_item->pp_children[i]->i_children >= 0 )
            print_playlist( p_intf, p_item->pp_children[i], i_level + 1 );
    }
}
