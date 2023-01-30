 *****************************************************************************/
static block_t *Packetize ( decoder_t *p_dec, block_t **pp_block )
{
    decoder_sys_t *p_sys = p_dec->p_sys;
    if( pp_block == NULL || *pp_block == NULL )
        return NULL;
    if( (*pp_block)->i_flags&(BLOCK_FLAG_DISCONTINUITY|BLOCK_FLAG_CORRUPTED) )
    {
        block_Release( *pp_block );
        return NULL;
    }
    block_t * p_block = *pp_block;
    uint8_t * p_indata = p_block->p_buffer + p_sys->i_offset;
    int i_inlen = p_block->i_buffer -  p_sys->i_offset;
    uint8_t * p_outdata;
    int i_outlen;
    if( p_sys->i_offset == i_inlen )
        goto out;
    p_sys->i_offset += av_parser_parse2( p_sys->p_parser_ctx, p_sys->p_codec_ctx,
                                         &p_outdata, &i_outlen, p_indata, i_inlen,
                                         p_block->i_pts, p_block->i_dts, -1);
    if( unlikely( i_outlen <= 0 || !p_outdata ) )
        goto out;
    block_t * p_ret = block_Alloc( i_outlen );
    if( unlikely ( !p_ret ) )
        goto out;
    memcpy( p_ret->p_buffer, p_outdata, i_outlen );
    p_ret->i_pts = p_block->i_pts;
    p_ret->i_dts = p_block->i_dts;
    p_block->i_pts = p_block->i_dts = VLC_TS_INVALID;
    return p_ret;
out:
    p_sys->i_offset = 0;
    block_Release( *pp_block );
    *pp_block = NULL;
    return NULL;
}
