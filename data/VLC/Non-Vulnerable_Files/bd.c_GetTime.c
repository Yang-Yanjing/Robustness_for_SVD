 */
static int64_t GetTime( demux_t *p_demux )
{
    demux_sys_t *p_sys = p_demux->p_sys;
    const int i_mpls = p_demux->info.i_title;
    const bd_mpls_t *p_mpls = p_sys->pp_mpls[i_mpls];
    const bd_mpls_play_item_t *p_item = &p_mpls->p_play_item[p_sys->i_play_item];
    const bd_clpi_t *p_clpi = p_sys->p_clpi;
    if( !p_clpi || p_clpi->i_ep_map <= 0 )
        return 0;
    /* */
    const bd_clpi_ep_map_t *p_ep_map = &p_clpi->p_ep_map[0];
    if( p_sys->i_clpi_ep < 0 || p_sys->i_clpi_ep >= p_ep_map->i_ep )
        return 0;
    const bd_clpi_ep_t *p_ep = &p_ep_map->p_ep[p_sys->i_clpi_ep];
    int64_t i_time = p_ep->i_pts / 2 - p_item->i_in_time +
                     ( p_sys->i_atc_current - p_sys->i_atc_initial ) / 300 / 2;
    for( int j = 0; j < p_sys->i_play_item; j++ )
    {
        const bd_mpls_play_item_t *p_item = &p_mpls->p_play_item[j];
        i_time += ( p_item->i_out_time - p_item->i_in_time );
    }
    return i_time * CLOCK_FREQ / BD_45KHZ;
}
