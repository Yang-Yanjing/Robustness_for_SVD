 *****************************************************************************/
static block_t *ParseMPEGBlock( decoder_t *p_dec, block_t *p_frag )
{
    decoder_sys_t *p_sys = p_dec->p_sys;
    block_t *p_pic = NULL;
    if( p_frag->p_buffer[3] == 0xB0 || p_frag->p_buffer[3] == 0xB1 || p_frag->p_buffer[3] == 0xB2 )
    {   /* VOS and USERDATA */
#if 0
        /* Remove VOS start/end code from the original stream */
        block_Release( p_frag );
#else
        /* Append the block for now since ts/ps muxers rely on VOL
         * being present in the stream */
        block_ChainLastAppend( &p_sys->pp_last, p_frag );
#endif
        return NULL;
    }
    if( p_frag->p_buffer[3] >= 0x20 && p_frag->p_buffer[3] <= 0x2f )
    {
        /* Copy the complete VOL */
        if( (size_t)p_dec->fmt_out.i_extra != p_frag->i_buffer )
        {
            p_dec->fmt_out.p_extra =
                xrealloc( p_dec->fmt_out.p_extra, p_frag->i_buffer );
            p_dec->fmt_out.i_extra = p_frag->i_buffer;
        }
        memcpy( p_dec->fmt_out.p_extra, p_frag->p_buffer, p_frag->i_buffer );
        ParseVOL( p_dec, &p_dec->fmt_out,
                  p_dec->fmt_out.p_extra, p_dec->fmt_out.i_extra );
#if 0
        /* Remove from the original stream */
        block_Release( p_frag );
#else
        /* Append the block for now since ts/ps muxers rely on VOL
         * being present in the stream */
        block_ChainLastAppend( &p_sys->pp_last, p_frag );
#endif
        return NULL;
    }
    else
    {
        if( !p_dec->fmt_out.i_extra )
        {
            msg_Warn( p_dec, "waiting for VOL" );
            block_Release( p_frag );
            return NULL;
        }
        /* Append the block */
        block_ChainLastAppend( &p_sys->pp_last, p_frag );
    }
    if( p_frag->p_buffer[3] == 0xb6 &&
        ParseVOP( p_dec, p_frag ) == VLC_SUCCESS )
    {
        /* We are dealing with a VOP */
        p_pic = block_ChainGather( p_sys->p_frame );
        p_pic->i_flags = p_sys->i_flags;
        p_pic->i_pts = p_sys->i_interpolated_pts;
        p_pic->i_dts = p_sys->i_interpolated_dts;
        /* Reset context */
        p_sys->p_frame = NULL;
        p_sys->pp_last = &p_sys->p_frame;
    }
    return p_pic;
}
