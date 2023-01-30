}
static void LoadClpi( demux_t *p_demux, const char *psz_name, int i_id )
{
    demux_sys_t *p_sys = p_demux->p_sys;
#if defined(BD_DEBUG)
    msg_Err( p_demux, "Loading %s", psz_name );
#endif
    block_t *p_block = LoadBlock( p_demux, psz_name );
    if( !p_block )
        goto error;
    /* */
    bd_clpi_t *p_clpi = malloc( sizeof(*p_clpi) );
    if( !p_clpi )
        goto error;
    /* */
    bs_t s;
    bs_init( &s, p_block->p_buffer, p_block->i_buffer );
    if( bd_clpi_Parse( p_clpi, &s, i_id ) )
        goto error;
#if defined(BD_DEBUG)
    msg_Err( p_demux, "CLPI: id=%d", p_clpi->i_id );
    msg_Err( p_demux, "CLPI: STC=%d", p_clpi->i_stc );
    for( int i = 0; i < p_clpi->i_stc; i++ )
        msg_Err( p_demux, "   STC[%d] pcr_pid=%d packet=%d start=%d end=%d",
                 i, p_clpi->p_stc[i].i_pcr_pid, (int)p_clpi->p_stc[i].i_packet,
                 (int)p_clpi->p_stc[i].i_start, (int)p_clpi->p_stc[i].i_end );
    msg_Err( p_demux, "CLPI: Stream=%d", p_clpi->i_stream );
    for( int i = 0; i < p_clpi->i_stream; i++ )
        msg_Err( p_demux, "   Stream[%d] pid=%d type=0x%x",
                 i, p_clpi->p_stream[i].i_pid, p_clpi->p_stream[i].i_type );
    msg_Err( p_demux, "CLPI: Ep Map=%d", p_clpi->i_ep_map );
    for( int i = 0; i < p_clpi->i_ep_map; i++ )
    {
        const bd_clpi_ep_map_t *p_ep_map = &p_clpi->p_ep_map[i];
        msg_Err( p_demux, "   Ep Map[%d] pid=%d type=0x%x entry_point=%d",
                 i, p_ep_map->i_pid, p_ep_map->i_type, p_ep_map->i_ep );
        for( int j = 0; j < p_ep_map->i_ep; j++ )
        {
            msg_Err( p_demux, "      Ep[%d] packet=%d pts=%d",
                     j, (int)p_ep_map->p_ep[j].i_packet, (int)p_ep_map->p_ep[j].i_pts );
        }
    }
#endif
    /* */
    TAB_APPEND( p_sys->i_clpi, p_sys->pp_clpi, p_clpi );
    /* */
    block_Release( p_block );
    return;
error:
    msg_Err( p_demux, "Failed loading %s", psz_name );
    if( p_block )
        block_Release( p_block );
}
