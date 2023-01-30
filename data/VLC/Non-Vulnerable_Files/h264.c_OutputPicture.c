}
static block_t *OutputPicture( decoder_t *p_dec )
{
    decoder_sys_t *p_sys = p_dec->p_sys;
    block_t *p_pic;
    if ( !p_sys->b_header && p_sys->i_recovery_frames != -1 )
    {
        if( p_sys->i_recovery_frames == 0 )
        {
            msg_Dbg( p_dec, "Recovery from SEI recovery point complete" );
            p_sys->b_header = true;
        }
        --p_sys->i_recovery_frames;
    }
    if( !p_sys->b_header && p_sys->i_recovery_frames == -1 &&
         p_sys->slice.i_frame_type != BLOCK_FLAG_TYPE_I)
        return NULL;
    const bool b_sps_pps_i = p_sys->slice.i_frame_type == BLOCK_FLAG_TYPE_I &&
                             p_sys->b_sps &&
                             p_sys->b_pps;
    if( b_sps_pps_i || p_sys->b_frame_sps || p_sys->b_frame_pps )
    {
        block_t *p_head = NULL;
        if( p_sys->p_frame->i_flags & BLOCK_FLAG_PRIVATE_AUD )
        {
            p_head = p_sys->p_frame;
            p_sys->p_frame = p_sys->p_frame->p_next;
        }
        block_t *p_list = NULL;
        for( int i = 0; i < SPS_MAX && (b_sps_pps_i || p_sys->b_frame_sps); i++ )
        {
            if( p_sys->pp_sps[i] )
                block_ChainAppend( &p_list, block_Duplicate( p_sys->pp_sps[i] ) );
        }
        for( int i = 0; i < PPS_MAX && (b_sps_pps_i || p_sys->b_frame_pps); i++ )
        {
            if( p_sys->pp_pps[i] )
                block_ChainAppend( &p_list, block_Duplicate( p_sys->pp_pps[i] ) );
        }
        if( b_sps_pps_i && p_list )
            p_sys->b_header = true;
        if( p_head )
            p_head->p_next = p_list;
        else
            p_head = p_list;
        block_ChainAppend( &p_head, p_sys->p_frame );
        p_pic = block_ChainGather( p_head );
    }
    else
    {
        p_pic = block_ChainGather( p_sys->p_frame );
    }
    p_pic->i_dts = p_sys->i_frame_dts;
    p_pic->i_pts = p_sys->i_frame_pts;
    p_pic->i_length = 0;    /* FIXME */
    p_pic->i_flags |= p_sys->slice.i_frame_type;
    p_pic->i_flags &= ~BLOCK_FLAG_PRIVATE_AUD;
    if( !p_sys->b_header )
        p_pic->i_flags |= BLOCK_FLAG_PREROLL;
    p_sys->slice.i_frame_type = 0;
    p_sys->p_frame = NULL;
    p_sys->i_frame_dts = VLC_TS_INVALID;
    p_sys->i_frame_pts = VLC_TS_INVALID;
    p_sys->b_frame_sps = false;
    p_sys->b_frame_pps = false;
    p_sys->b_slice = false;
    /* CC */
    p_sys->i_cc_pts = p_pic->i_pts;
    p_sys->i_cc_dts = p_pic->i_dts;
    p_sys->i_cc_flags = p_pic->i_flags;
    p_sys->cc = p_sys->cc_next;
    cc_Flush( &p_sys->cc_next );
    return p_pic;
}
