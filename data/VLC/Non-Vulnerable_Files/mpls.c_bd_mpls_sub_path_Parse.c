}
void bd_mpls_sub_path_Parse( bd_mpls_sub_path_t *p_path, bs_t *s )
{
    const uint32_t i_length = bs_read( s, 32 );
    const int i_start = bs_pos( s ) / 8;
    bs_skip( s, 8 );
    p_path->i_type = bs_read( s, 8 );
    bs_skip( s, 15 );
    p_path->b_repeat = bs_read( s, 1 );
    bs_skip( s, 8 );
    p_path->i_item = bs_read( s, 8 );
    for( int j = 0; j < p_path->i_item; j++ )
    {
        const int i_length = bs_read( s, 16 );
        const int i_start = bs_pos( s ) / 8;
        /* TODO */
        bs_skip( s, 8 * ( i_start + i_length ) - bs_pos( s ) );
    }
    bs_skip( s, 8 * ( i_start + i_length ) - bs_pos( s ) );
}
