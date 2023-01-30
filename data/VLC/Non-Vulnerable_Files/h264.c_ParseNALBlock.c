 *****************************************************************************/
static block_t *ParseNALBlock( decoder_t *p_dec, bool *pb_ts_used, block_t *p_frag )
{
    decoder_sys_t *p_sys = p_dec->p_sys;
    block_t *p_pic = NULL;
    const int i_nal_ref_idc = (p_frag->p_buffer[4] >> 5)&0x03;
    const int i_nal_type = p_frag->p_buffer[4]&0x1f;
    const mtime_t i_frag_dts = p_frag->i_dts;
    const mtime_t i_frag_pts = p_frag->i_pts;
    if( p_sys->b_slice && ( !p_sys->b_sps || !p_sys->b_pps ) )
    {
        block_ChainRelease( p_sys->p_frame );
        msg_Warn( p_dec, "waiting for SPS/PPS" );
        /* Reset context */
        p_sys->slice.i_frame_type = 0;
        p_sys->p_frame = NULL;
        p_sys->b_frame_sps = false;
        p_sys->b_frame_pps = false;
        p_sys->b_slice = false;
        cc_Flush( &p_sys->cc_next );
    }
    if( ( !p_sys->b_sps || !p_sys->b_pps ) &&
        i_nal_type >= NAL_SLICE && i_nal_type <= NAL_SLICE_IDR )
    {
        p_sys->b_slice = true;
        /* Fragment will be discarded later on */
    }
    else if( i_nal_type >= NAL_SLICE && i_nal_type <= NAL_SLICE_IDR )
    {
        slice_t slice;
        bool  b_new_picture;
        ParseSlice( p_dec, &b_new_picture, &slice, i_nal_ref_idc, i_nal_type, p_frag );
        /* */
        if( b_new_picture && p_sys->b_slice )
            p_pic = OutputPicture( p_dec );
        /* */
        p_sys->slice = slice;
        p_sys->b_slice = true;
    }
    else if( i_nal_type == NAL_SPS )
    {
        if( p_sys->b_slice )
            p_pic = OutputPicture( p_dec );
        p_sys->b_frame_sps = true;
        PutSPS( p_dec, p_frag );
        /* Do not append the SPS because we will insert it on keyframes */
        p_frag = NULL;
    }
    else if( i_nal_type == NAL_PPS )
    {
        if( p_sys->b_slice )
            p_pic = OutputPicture( p_dec );
        p_sys->b_frame_pps = true;
        PutPPS( p_dec, p_frag );
        /* Do not append the PPS because we will insert it on keyframes */
        p_frag = NULL;
    }
    else if( i_nal_type == NAL_AU_DELIMITER ||
             i_nal_type == NAL_SEI ||
             ( i_nal_type >= 13 && i_nal_type <= 18 ) )
    {
        if( p_sys->b_slice )
            p_pic = OutputPicture( p_dec );
        /* Parse SEI for CC support */
        if( i_nal_type == NAL_SEI )
        {
            ParseSei( p_dec, p_frag );
        }
        else if( i_nal_type == NAL_AU_DELIMITER )
        {
            if( p_sys->p_frame && (p_sys->p_frame->i_flags & BLOCK_FLAG_PRIVATE_AUD) )
            {
                block_Release( p_frag );
                p_frag = NULL;
            }
            else
            {
                p_frag->i_flags |= BLOCK_FLAG_PRIVATE_AUD;
            }
        }
    }
    /* Append the block */
    if( p_frag )
        block_ChainAppend( &p_sys->p_frame, p_frag );
    *pb_ts_used = false;
    if( p_sys->i_frame_dts <= VLC_TS_INVALID &&
        p_sys->i_frame_pts <= VLC_TS_INVALID )
    {
        p_sys->i_frame_dts = i_frag_dts;
        p_sys->i_frame_pts = i_frag_pts;
        *pb_ts_used = true;
    }
    return p_pic;
}
