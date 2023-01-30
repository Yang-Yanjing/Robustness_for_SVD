 *****************************************************************************/
static subpicture_t *ProcessPacket( decoder_t *p_dec, kate_packet *p_kp,
                            block_t **pp_block )
{
    decoder_sys_t *p_sys = p_dec->p_sys;
    block_t *p_block = *pp_block;
    subpicture_t *p_buf = NULL;
    /* Date management */
    if( p_block->i_pts > VLC_TS_INVALID && p_block->i_pts != p_sys->i_pts )
    {
        p_sys->i_pts = p_block->i_pts;
    }
    *pp_block = NULL; /* To avoid being fed the same packet again */
#ifdef ENABLE_PACKETIZER
    if( p_sys->b_packetizer )
    {
        /* Date management */
        p_block->i_dts = p_block->i_pts = p_sys->i_pts;
        if( p_sys->i_headers >= p_sys->i_num_headers )
            p_block->i_length = p_sys->i_pts - p_block->i_pts;
        else
            p_block->i_length = 0;
        p_buf = p_block;
    }
    else
#endif
    {
        p_buf = DecodePacket( p_dec, p_kp, p_block );
        if( p_block )
            block_Release( p_block );
    }
    return p_buf;
}
