#define SPU_HEADER_LEN 1
static block_t *Reassemble( decoder_t *p_dec, block_t *p_block )
{
    decoder_sys_t *p_sys = p_dec->p_sys;
    if( p_block->i_buffer < SPU_HEADER_LEN )
    {
        msg_Dbg( p_dec, "invalid packet header (size %zu < %u)" ,
                 p_block->i_buffer, SPU_HEADER_LEN );
        block_Release( p_block );
        return NULL;
    }
    /* From the scant data on the format, there is only only way known
     * to detect the first packet in a subtitle.  The first packet
     * seems to have a valid PTS while later packets for the same
     * image don't. */
    if( p_sys->i_state == SUBTITLE_BLOCK_EMPTY && p_block->i_pts <= VLC_TS_INVALID )
    {
        msg_Warn( p_dec, "first packet expected but no PTS present");
        return NULL;
    }
    p_block->p_buffer += SPU_HEADER_LEN;
    p_block->i_buffer -= SPU_HEADER_LEN;
    /* First packet in the subtitle block */
    if( p_sys->i_state == SUBTITLE_BLOCK_EMPTY ) ParseHeader( p_dec, p_block );
    block_ChainAppend( &p_sys->p_spu, p_block );
    p_sys->p_spu = block_ChainGather( p_sys->p_spu );
    if( p_sys->p_spu->i_buffer >= p_sys->i_spu_size )
    {
        block_t *p_spu = p_sys->p_spu;
        if( p_spu->i_buffer != p_sys->i_spu_size )
        {
            msg_Warn( p_dec, "SPU packets size=%zu should be %zu",
                      p_spu->i_buffer, p_sys->i_spu_size );
        }
        msg_Dbg( p_dec, "subtitle packet complete, size=%zuu", p_spu->i_buffer);
        ParseMetaInfo( p_dec, p_spu );
        p_sys->i_state = SUBTITLE_BLOCK_EMPTY;
        p_sys->p_spu = 0;
        return p_spu;
    }
    else
    {
        /* Not last block in subtitle, so wait for another. */
        p_sys->i_state = SUBTITLE_BLOCK_PARTIAL;
    }
    return NULL;
}
