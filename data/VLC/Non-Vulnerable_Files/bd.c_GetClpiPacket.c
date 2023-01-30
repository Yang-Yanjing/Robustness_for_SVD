}
static int64_t GetClpiPacket( demux_t *p_demux, int *pi_ep, const bd_mpls_clpi_t *p_mpls_clpi, int64_t i_time /* in 45kHz */ )
{
    demux_sys_t *p_sys = p_demux->p_sys;
    const bd_clpi_t *p_clpi = p_sys->p_clpi;
    assert( p_clpi );
    if( p_clpi->i_ep_map <= 0 )
        return -1;
    const bd_clpi_ep_map_t *p_ep_map = &p_clpi->p_ep_map[0];
    if( p_mpls_clpi->i_stc_id < 0 || p_mpls_clpi->i_stc_id >= p_clpi->i_stc )
        return -1;
    const bd_clpi_stc_t *p_stc = &p_clpi->p_stc[p_mpls_clpi->i_stc_id];
#if 0
    /* Not sure it is right */
    if( i_time < p_stc->i_start || i_time > p_stc->i_end )
        return -1;
#endif
    const int64_t i_packet = p_stc->i_packet;
    int i_ep;
    for( i_ep = 0; i_ep < p_ep_map->i_ep; i_ep++ )
    {
        if( p_ep_map->p_ep[i_ep].i_packet >= i_packet )
            break;
    }
    if( i_ep >= p_ep_map->i_ep )
        return -1;
    for( ; i_ep < p_ep_map->i_ep; i_ep++ )
    {
        const bd_clpi_ep_t *p_ep = &p_ep_map->p_ep[i_ep];
        const bd_clpi_ep_t *p_ep_next = &p_ep_map->p_ep[i_ep+1];
        if( i_ep+1 < p_ep_map->i_ep && p_ep_next->i_pts / 2 > i_time )
            break;
        if( p_ep->i_pts / 2 >= i_time )
            break;
    }
    if( i_ep >= p_ep_map->i_ep )
        return -1;
    /* */
    if( pi_ep )
        *pi_ep = i_ep;
    return p_ep_map->p_ep[i_ep].i_packet;
}
