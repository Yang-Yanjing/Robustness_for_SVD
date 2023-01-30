 *****************************************************************************/
static block_t *Packetize( decoder_t *p_dec, block_t **pp_block )
{
    decoder_sys_t *p_sys = p_dec->p_sys;
    block_t       *p_block;
    int           i_frame_length, i_bits;
    p_block = Parse( p_dec, &i_frame_length, &i_bits, pp_block, true );
    if( !p_block )
        return NULL;
    p_block->i_pts = p_block->i_dts = date_Get( &p_sys->end_date );
    p_block->i_length = date_Increment( &p_sys->end_date, i_frame_length ) - p_block->i_pts;
    /* Just pass on the incoming frame */
    return p_block;
}
