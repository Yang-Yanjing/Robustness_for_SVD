 *****************************************************************************/
static block_t *SendPacket( decoder_t *p_dec, block_t *p_block )
{
    decoder_sys_t *p_sys = p_dec->p_sys;
    /* Date management */
    p_block->i_dts = p_block->i_pts = date_Get( &p_sys->end_date );
    p_block->i_length =
        date_Increment( &p_sys->end_date,
                            p_sys->p_header->frame_size ) -
        p_block->i_pts;
    return p_block;
}
