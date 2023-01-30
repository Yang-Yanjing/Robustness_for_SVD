}
static void PutPPS( decoder_t *p_dec, block_t *p_frag )
{
    decoder_sys_t *p_sys = p_dec->p_sys;
    bs_t s;
    uint32_t i_pps_id;
    uint32_t i_sps_id;
    bs_init( &s, &p_frag->p_buffer[5], p_frag->i_buffer - 5 );
    i_pps_id = bs_read_ue( &s ); // pps id
    i_sps_id = bs_read_ue( &s ); // sps id
    if( i_pps_id >= PPS_MAX || i_sps_id >= SPS_MAX )
    {
        msg_Warn( p_dec, "invalid PPS (pps_id=%d sps_id=%d)", i_pps_id, i_sps_id );
        block_Release( p_frag );
        return;
    }
    bs_skip( &s, 1 ); // entropy coding mode flag
    p_sys->i_pic_order_present_flag = bs_read( &s, 1 );
    /* TODO */
    /* We have a new PPS */
    if( !p_sys->b_pps )
        msg_Dbg( p_dec, "found NAL_PPS (pps_id=%d sps_id=%d)", i_pps_id, i_sps_id );
    p_sys->b_pps = true;
    if( p_sys->pp_pps[i_pps_id] )
        block_Release( p_sys->pp_pps[i_pps_id] );
    p_sys->pp_pps[i_pps_id] = p_frag;
}
