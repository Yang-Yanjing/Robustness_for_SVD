 ****************************************************************************/
static block_t *PacketizeAVC1( decoder_t *p_dec, block_t **pp_block )
{
    decoder_sys_t *p_sys = p_dec->p_sys;
    block_t       *p_block;
    block_t       *p_ret = NULL;
    uint8_t       *p;
    if( !pp_block || !*pp_block )
        return NULL;
    if( (*pp_block)->i_flags&(BLOCK_FLAG_DISCONTINUITY|BLOCK_FLAG_CORRUPTED) )
    {
        block_Release( *pp_block );
        return NULL;
    }
    p_block = *pp_block;
    *pp_block = NULL;
    for( p = p_block->p_buffer; p < &p_block->p_buffer[p_block->i_buffer]; )
    {
        block_t *p_pic;
        bool b_dummy;
        int i_size = 0;
        int i;
        for( i = 0; i < p_sys->i_avcC_length_size; i++ )
        {
            i_size = (i_size << 8) | (*p++);
        }
        if( i_size <= 0 ||
            i_size > ( p_block->p_buffer + p_block->i_buffer - p ) )
        {
            msg_Err( p_dec, "Broken frame : size %d is too big", i_size );
            break;
        }
        block_t *p_part = CreateAnnexbNAL( p_dec, p, i_size );
        if( !p_part )
            break;
        p_part->i_dts = p_block->i_dts;
        p_part->i_pts = p_block->i_pts;
        /* Parse the NAL */
        if( ( p_pic = ParseNALBlock( p_dec, &b_dummy, p_part ) ) )
        {
            block_ChainAppend( &p_ret, p_pic );
        }
        p += i_size;
    }
    block_Release( p_block );
    return p_ret;
}
