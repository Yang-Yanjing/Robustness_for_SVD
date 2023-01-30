}
static block_t *PacketizeParse(void *p_private, bool *pb_ts_used, block_t *p_block)
{
    decoder_t *p_dec = p_private;
    decoder_sys_t *p_sys = p_dec->p_sys;
    block_t * p_nal = NULL;
    while (p_block->i_buffer > 5 && p_block->p_buffer[p_block->i_buffer-1] == 0x00 )
        p_block->i_buffer--;
    bs_t bs;
    bs_init(&bs, p_block->p_buffer+4, p_block->i_buffer-4);
    /* Get NALU type */
    uint32_t forbidden_zero_bit = bs_read1(&bs);
    if (forbidden_zero_bit)
    {
        msg_Err(p_dec,"Forbidden zero bit not null, corrupted NAL");
        p_sys->p_frame = NULL;
        p_sys->b_vcl = false;
        return NULL;
    }
    uint32_t nalu_type = bs_read(&bs,6);
    bs_skip(&bs, 9);
    if (nalu_type < VPS)
    {
        /* NAL is a VCL NAL */
        p_sys->b_vcl = true;
        uint32_t first_slice_in_pic = bs_read1(&bs);
        if (first_slice_in_pic && p_sys->p_frame)
        {
            p_nal = block_ChainGather(p_sys->p_frame);
            p_sys->p_frame = NULL;
        }
        block_ChainAppend(&p_sys->p_frame, p_block);
    }
    else
    {
        if (p_sys->b_vcl)
        {
            p_nal = block_ChainGather(p_sys->p_frame);
            p_nal->p_next = p_block;
            p_sys->p_frame = NULL;
            p_sys->b_vcl =false;
        }
        else
            p_nal = p_block;
    }
    *pb_ts_used = false;
    return p_nal;
}
