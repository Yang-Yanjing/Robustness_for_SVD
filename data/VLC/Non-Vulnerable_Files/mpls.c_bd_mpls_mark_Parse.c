}
void bd_mpls_mark_Parse( bd_mpls_mark_t *p_mark, bs_t *s )
{
    bs_skip( s, 8 );
    p_mark->i_type = bs_read( s, 8 );
    p_mark->i_play_item_id = bs_read( s, 16 );
    p_mark->i_time = bs_read( s, 32 );
    p_mark->i_entry_es_pid = bs_read( s, 16 );
    bs_skip( s, 32 );
}
