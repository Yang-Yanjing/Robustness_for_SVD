}
void bd_mpls_Clean( bd_mpls_t *p_mpls )
{
    for( int i = 0; i < p_mpls->i_play_item; i++ )
        bd_mpls_play_item_Clean( &p_mpls->p_play_item[i] );
    free( p_mpls->p_play_item );
    free( p_mpls->p_sub_path );
    free( p_mpls->p_mark );
}
