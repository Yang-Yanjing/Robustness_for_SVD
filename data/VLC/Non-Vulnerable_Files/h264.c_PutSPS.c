}
static void PutSPS( decoder_t *p_dec, block_t *p_frag )
{
    decoder_sys_t *p_sys = p_dec->p_sys;
    uint8_t *pb_dec = NULL;
    int     i_dec = 0;
    bs_t s;
    int i_tmp;
    uint32_t i_sps_id;
    CreateDecodedNAL( &pb_dec, &i_dec, &p_frag->p_buffer[5],
                     p_frag->i_buffer - 5 );
    bs_init( &s, pb_dec, i_dec );
    int i_profile_idc = bs_read( &s, 8 );
    p_dec->fmt_out.i_profile = i_profile_idc;
    /* Skip constraint_set0123, reserved(4) */
    bs_skip( &s, 1+1+1+1 + 4 );
    p_dec->fmt_out.i_level = bs_read( &s, 8 );
    /* sps id */
    i_sps_id = bs_read_ue( &s );
    if( i_sps_id >= SPS_MAX )
    {
        msg_Warn( p_dec, "invalid SPS (sps_id=%u)", i_sps_id );
        free( pb_dec );
        block_Release( p_frag );
        return;
    }
    if( i_profile_idc == 100 || i_profile_idc == 110 ||
        i_profile_idc == 122 || i_profile_idc == 244 ||
        i_profile_idc ==  44 || i_profile_idc ==  83 ||
        i_profile_idc ==  86 )
    {
        /* chroma_format_idc */
        const int i_chroma_format_idc = bs_read_ue( &s );
        if( i_chroma_format_idc == 3 )
            bs_skip( &s, 1 ); /* separate_colour_plane_flag */
        /* bit_depth_luma_minus8 */
        bs_read_ue( &s );
        /* bit_depth_chroma_minus8 */
        bs_read_ue( &s );
        /* qpprime_y_zero_transform_bypass_flag */
        bs_skip( &s, 1 );
        /* seq_scaling_matrix_present_flag */
        i_tmp = bs_read( &s, 1 );
        if( i_tmp )
        {
            for( int i = 0; i < ((3 != i_chroma_format_idc) ? 8 : 12); i++ )
            {
                /* seq_scaling_list_present_flag[i] */
                i_tmp = bs_read( &s, 1 );
                if( !i_tmp )
                    continue;
                const int i_size_of_scaling_list = (i < 6 ) ? 16 : 64;
                /* scaling_list (...) */
                int i_lastscale = 8;
                int i_nextscale = 8;
                for( int j = 0; j < i_size_of_scaling_list; j++ )
                {
                    if( i_nextscale != 0 )
                    {
                        /* delta_scale */
                        i_tmp = bs_read_se( &s );
                        i_nextscale = ( i_lastscale + i_tmp + 256 ) % 256;
                        /* useDefaultScalingMatrixFlag = ... */
                    }
                    /* scalinglist[j] */
                    i_lastscale = ( i_nextscale == 0 ) ? i_lastscale : i_nextscale;
                }
            }
        }
    }
    /* Skip i_log2_max_frame_num */
    p_sys->i_log2_max_frame_num = bs_read_ue( &s );
    if( p_sys->i_log2_max_frame_num > 12)
        p_sys->i_log2_max_frame_num = 12;
    /* Read poc_type */
    p_sys->i_pic_order_cnt_type = bs_read_ue( &s );
    if( p_sys->i_pic_order_cnt_type == 0 )
    {
        /* skip i_log2_max_poc_lsb */
        p_sys->i_log2_max_pic_order_cnt_lsb = bs_read_ue( &s );
        if( p_sys->i_log2_max_pic_order_cnt_lsb > 12 )
            p_sys->i_log2_max_pic_order_cnt_lsb = 12;
    }
    else if( p_sys->i_pic_order_cnt_type == 1 )
    {
        int i_cycle;
        /* skip b_delta_pic_order_always_zero */
        p_sys->i_delta_pic_order_always_zero_flag = bs_read( &s, 1 );
        /* skip i_offset_for_non_ref_pic */
        bs_read_se( &s );
        /* skip i_offset_for_top_to_bottom_field */
        bs_read_se( &s );
        /* read i_num_ref_frames_in_poc_cycle */
        i_cycle = bs_read_ue( &s );
        if( i_cycle > 256 ) i_cycle = 256;
        while( i_cycle > 0 )
        {
            /* skip i_offset_for_ref_frame */
            bs_read_se(&s );
            i_cycle--;
        }
    }
    /* i_num_ref_frames */
    bs_read_ue( &s );
    /* b_gaps_in_frame_num_value_allowed */
    bs_skip( &s, 1 );
    /* Read size */
    p_dec->fmt_out.video.i_width  = 16 * ( bs_read_ue( &s ) + 1 );
    p_dec->fmt_out.video.i_height = 16 * ( bs_read_ue( &s ) + 1 );
    /* b_frame_mbs_only */
    p_sys->b_frame_mbs_only = bs_read( &s, 1 );
    p_dec->fmt_out.video.i_height *=  ( 2 - p_sys->b_frame_mbs_only );
    if( p_sys->b_frame_mbs_only == 0 )
    {
        bs_skip( &s, 1 );
    }
    /* b_direct8x8_inference */
    bs_skip( &s, 1 );
    /* crop */
    i_tmp = bs_read( &s, 1 );
    if( i_tmp )
    {
        /* left */
        bs_read_ue( &s );
        /* right */
        bs_read_ue( &s );
        /* top */
        bs_read_ue( &s );
        /* bottom */
        bs_read_ue( &s );
    }
    /* vui */
    i_tmp = bs_read( &s, 1 );
    if( i_tmp )
    {
        /* read the aspect ratio part if any */
        i_tmp = bs_read( &s, 1 );
        if( i_tmp )
        {
            static const struct { int w, h; } sar[17] =
            {
                { 0,   0 }, { 1,   1 }, { 12, 11 }, { 10, 11 },
                { 16, 11 }, { 40, 33 }, { 24, 11 }, { 20, 11 },
                { 32, 11 }, { 80, 33 }, { 18, 11 }, { 15, 11 },
                { 64, 33 }, { 160,99 }, {  4,  3 }, {  3,  2 },
                {  2,  1 },
            };
            int i_sar = bs_read( &s, 8 );
            int w, h;
            if( i_sar < 17 )
            {
                w = sar[i_sar].w;
                h = sar[i_sar].h;
            }
            else if( i_sar == 255 )
            {
                w = bs_read( &s, 16 );
                h = bs_read( &s, 16 );
            }
            else
            {
                w = 0;
                h = 0;
            }
            if( w != 0 && h != 0 )
            {
                p_dec->fmt_out.video.i_sar_num = w;
                p_dec->fmt_out.video.i_sar_den = h;
            }
            else
            {
                p_dec->fmt_out.video.i_sar_num = 1;
                p_dec->fmt_out.video.i_sar_den = 1;
            }
        }
    }
    free( pb_dec );
    /* We have a new SPS */
    if( !p_sys->b_sps )
        msg_Dbg( p_dec, "found NAL_SPS (sps_id=%d)", i_sps_id );
    p_sys->b_sps = true;
    if( p_sys->pp_sps[i_sps_id] )
        block_Release( p_sys->pp_sps[i_sps_id] );
    p_sys->pp_sps[i_sps_id] = p_frag;
}
