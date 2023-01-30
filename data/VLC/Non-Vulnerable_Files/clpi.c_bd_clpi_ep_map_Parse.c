}
int bd_clpi_ep_map_Parse( bd_clpi_ep_map_t *p_ep_map,
                          bs_t *s, const int i_ep_map_start )
{
    p_ep_map->i_pid = bs_read( s, 16 );
    bs_skip( s, 10 );
    p_ep_map->i_type = bs_read( s, 4 );
    const int i_coarse = bs_read( s, 16 );
    const int i_fine = bs_read( s, 18 );
    const uint32_t i_coarse_start = bs_read( s, 32 );
    p_ep_map->i_ep = i_fine;
    p_ep_map->p_ep = calloc( i_fine, sizeof(*p_ep_map->p_ep) );
    if( !p_ep_map->p_ep )
        return VLC_EGENERIC;
    bs_t cs = *s;
    bs_skip( &cs, 8*(i_ep_map_start + i_coarse_start) - bs_pos( s ) );
    const uint32_t i_fine_start = bs_read( &cs, 32 );
    for( int i = 0; i < i_coarse; i++ )
    {
        const int      i_fine_id = bs_read( &cs, 18 );
        const int      i_pts = bs_read( &cs, 14 );
        const uint32_t i_packet = bs_read( &cs, 32 );
        for( int j = i_fine_id; j < p_ep_map->i_ep; j++ )
        {
            p_ep_map->p_ep[j].i_pts = (int64_t)(i_pts & ~1) << 19;
            p_ep_map->p_ep[j].i_packet = i_packet & ~( (1 << 17) - 1 );
        }
    }
    bs_t fs = *s;
    bs_skip( &fs, 8*(i_ep_map_start + i_coarse_start + i_fine_start) - bs_pos( s ) );
    for( int i = 0; i < i_fine; i++ )
    {
        const bool b_angle_point = bs_read( &fs, 1 );
        bs_skip( &fs, 3 );  /* I end position offset */
        const int i_pts = bs_read( &fs, 11 );
        const int i_packet = bs_read( &fs, 17 );
        p_ep_map->p_ep[i].b_angle_point = b_angle_point;
        p_ep_map->p_ep[i].i_pts |= i_pts << 9;
        p_ep_map->p_ep[i].i_packet |= i_packet;
    }
    return VLC_SUCCESS;
}
