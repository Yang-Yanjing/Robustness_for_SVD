 *****************************************************************************/
static block_t *ParseMPEGBlock( decoder_t *p_dec, block_t *p_frag )
{
    decoder_sys_t *p_sys = p_dec->p_sys;
    block_t *p_pic = NULL;
    /*
     * Check if previous picture is finished
     */
    if( ( p_sys->b_frame_slice &&
          (p_frag->p_buffer[3] == 0x00 || p_frag->p_buffer[3] > 0xaf) ) &&
          p_sys->p_seq == NULL )
    {
        /* We have a picture but without a sequence header we can't
         * do anything */
        msg_Dbg( p_dec, "waiting for sequence start" );
        if( p_sys->p_frame ) block_ChainRelease( p_sys->p_frame );
        p_sys->p_frame = NULL;
        p_sys->pp_last = &p_sys->p_frame;
        p_sys->b_frame_slice = false;
    }
    else if( p_sys->b_frame_slice &&
             (p_frag->p_buffer[3] == 0x00 || p_frag->p_buffer[3] > 0xaf) )
    {
        const bool b_eos = p_frag->p_buffer[3] == 0xb7;
        mtime_t i_duration;
        if( b_eos )
        {
            block_ChainLastAppend( &p_sys->pp_last, p_frag );
            p_frag = NULL;
        }
        p_pic = block_ChainGather( p_sys->p_frame );
        if( b_eos )
            p_pic->i_flags |= BLOCK_FLAG_END_OF_SEQUENCE;
        i_duration = (mtime_t)( CLOCK_FREQ * p_sys->i_frame_rate_base /
                                p_sys->i_frame_rate );
        if( !p_sys->b_seq_progressive && p_sys->i_picture_structure != 0x03 )
        {
            i_duration /= 2;
        }
        if( p_sys->b_seq_progressive )
        {
            if( p_sys->i_top_field_first == 0 &&
                p_sys->i_repeat_first_field == 1 )
            {
                i_duration *= 2;
            }
            else if( p_sys->i_top_field_first == 1 &&
                     p_sys->i_repeat_first_field == 1 )
            {
                i_duration *= 3;
            }
        }
        else
        {
            if( p_sys->i_picture_structure == 0x03 )
            {
                if( p_sys->i_progressive_frame && p_sys->i_repeat_first_field )
                {
                    i_duration += i_duration / 2;
                }
            }
        }
        if( p_sys->b_low_delay || p_sys->i_picture_type == 0x03 )
        {
            /* Trivial case (DTS == PTS) */
            /* Correct interpolated dts when we receive a new pts/dts */
            if( p_sys->i_pts > VLC_TS_INVALID )
                p_sys->i_interpolated_dts = p_sys->i_pts;
            if( p_sys->i_dts > VLC_TS_INVALID )
                p_sys->i_interpolated_dts = p_sys->i_dts;
        }
        else
        {
            /* Correct interpolated dts when we receive a new pts/dts */
            if(p_sys->i_last_ref_pts > VLC_TS_INVALID && !p_sys->b_second_field)
                p_sys->i_interpolated_dts = p_sys->i_last_ref_pts;
            if( p_sys->i_dts > VLC_TS_INVALID )
                p_sys->i_interpolated_dts = p_sys->i_dts;
            if( !p_sys->b_second_field )
                p_sys->i_last_ref_pts = p_sys->i_pts;
        }
        p_pic->i_dts = p_sys->i_interpolated_dts;
        p_sys->i_interpolated_dts += i_duration;
        /* Set PTS only if we have a B frame or if it comes from the stream */
        if( p_sys->i_pts > VLC_TS_INVALID )
        {
            p_pic->i_pts = p_sys->i_pts;
        }
        else if( p_sys->i_picture_type == 0x03 )
        {
            p_pic->i_pts = p_pic->i_dts;
        }
        else
        {
            p_pic->i_pts = VLC_TS_INVALID;
        }
        switch ( p_sys->i_picture_type )
        {
        case 0x01:
            p_pic->i_flags |= BLOCK_FLAG_TYPE_I;
            break;
        case 0x02:
            p_pic->i_flags |= BLOCK_FLAG_TYPE_P;
            break;
        case 0x03:
            p_pic->i_flags |= BLOCK_FLAG_TYPE_B;
            break;
        }
        p_pic->i_length = p_sys->i_interpolated_dts - p_pic->i_dts;
#if 0
        msg_Dbg( p_dec, "pic: type=%d dts=%"PRId64" pts-dts=%"PRId64,
        p_sys->i_picture_type, p_pic->i_dts, p_pic->i_pts - p_pic->i_dts);
#endif
        /* Reset context */
        p_sys->p_frame = NULL;
        p_sys->pp_last = &p_sys->p_frame;
        p_sys->b_frame_slice = false;
        if( p_sys->i_picture_structure != 0x03 )
        {
            p_sys->b_second_field = !p_sys->b_second_field;
        }
        else
        {
            p_sys->b_second_field = 0;
        }
        /* CC */
        p_sys->b_cc_reset = true;
        p_sys->i_cc_pts = p_pic->i_pts;
        p_sys->i_cc_dts = p_pic->i_dts;
        p_sys->i_cc_flags = p_pic->i_flags;
    }
    if( !p_pic && p_sys->b_cc_reset )
    {
        p_sys->b_cc_reset = false;
        cc_Flush( &p_sys->cc );
    }
    if( !p_frag )
        return p_pic;
    /*
     * Check info of current fragment
     */
    if( p_frag->p_buffer[3] == 0xb8 )
    {
        /* Group start code */
        if( p_sys->p_seq &&
            p_sys->i_seq_old > p_sys->i_frame_rate/p_sys->i_frame_rate_base )
        {
            /* Useful for mpeg1: repeat sequence header every second */
            block_ChainLastAppend( &p_sys->pp_last, block_Duplicate( p_sys->p_seq ) );
            if( p_sys->p_ext )
            {
                block_ChainLastAppend( &p_sys->pp_last, block_Duplicate( p_sys->p_ext ) );
            }
            p_sys->i_seq_old = 0;
        }
    }
    else if( p_frag->p_buffer[3] == 0xb3 && p_frag->i_buffer >= 8 )
    {
        /* Sequence header code */
        static const int code_to_frame_rate[16][2] =
        {
            { 1, 1 },  /* invalid */
            { 24000, 1001 }, { 24, 1 }, { 25, 1 },       { 30000, 1001 },
            { 30, 1 },       { 50, 1 }, { 60000, 1001 }, { 60, 1 },
            /* Unofficial 15fps from Xing*/
            { 15, 1001 },
            /* Unofficial economy rates from libmpeg3 */
            { 5000, 1001 }, { 1000, 1001 }, { 12000, 1001 }, { 15000, 1001 },
            { 1, 1 },  { 1, 1 }  /* invalid */
        };
        if( p_sys->p_seq ) block_Release( p_sys->p_seq );
        if( p_sys->p_ext ) block_Release( p_sys->p_ext );
        p_sys->p_seq = block_Duplicate( p_frag );
        p_sys->i_seq_old = 0;
        p_sys->p_ext = NULL;
        p_dec->fmt_out.video.i_width =
            ( p_frag->p_buffer[4] << 4)|(p_frag->p_buffer[5] >> 4 );
        p_dec->fmt_out.video.i_height =
            ( (p_frag->p_buffer[5]&0x0f) << 8 )|p_frag->p_buffer[6];
        p_sys->i_aspect_ratio_info = p_frag->p_buffer[7] >> 4;
        /* TODO: MPEG1 aspect ratio */
        p_sys->i_frame_rate = code_to_frame_rate[p_frag->p_buffer[7]&0x0f][0];
        p_sys->i_frame_rate_base =
            code_to_frame_rate[p_frag->p_buffer[7]&0x0f][1];
        p_dec->fmt_out.video.i_frame_rate = p_sys->i_frame_rate;
        p_dec->fmt_out.video.i_frame_rate_base = p_sys->i_frame_rate_base;
        p_sys->b_seq_progressive = true;
        p_sys->b_low_delay = true;
        if ( !p_sys->b_inited )
        {
            msg_Dbg( p_dec, "size %dx%d fps=%.3f",
                 p_dec->fmt_out.video.i_width, p_dec->fmt_out.video.i_height,
                 p_sys->i_frame_rate / (float)p_sys->i_frame_rate_base );
            p_sys->b_inited = 1;
        }
    }
    else if( p_frag->p_buffer[3] == 0xb5 )
    {
        int i_type = p_frag->p_buffer[4] >> 4;
        /* Extension start code */
        if( i_type == 0x01 )
        {
#if 0
            static const int mpeg2_aspect[16][2] =
            {
                {0,1}, {1,1}, {4,3}, {16,9}, {221,100},
                {0,1}, {0,1}, {0,1}, {0,1}, {0,1}, {0,1}, {0,1}, {0,1}, {0,1},
                {0,1}, {0,1}
            };
#endif
            /* sequence extension */
            if( p_sys->p_ext) block_Release( p_sys->p_ext );
            p_sys->p_ext = block_Duplicate( p_frag );
            if( p_frag->i_buffer >= 10 )
            {
                p_sys->b_seq_progressive =
                    p_frag->p_buffer[5]&0x08 ? true : false;
                p_sys->b_low_delay =
                    p_frag->p_buffer[9]&0x80 ? true : false;
            }
            /* Do not set aspect ratio : in case we're transcoding,
             * transcode will take our fmt_out as a fmt_in to libmpeg2.
             * libmpeg2.c will then believe that the user has requested
             * a specific aspect ratio, which she hasn't. Thus in case
             * of aspect ratio change, we're screwed. --Meuuh
             */
#if 0
            p_dec->fmt_out.video.i_sar_num =
                mpeg2_aspect[p_sys->i_aspect_ratio_info][0] *
                p_dec->fmt_out.video.i_height;
            p_dec->fmt_out.video.i_sar_den =
                mpeg2_aspect[p_sys->i_aspect_ratio_info][1] *
                p_dec->fmt_out.video.i_width;
#endif
        }
        else if( i_type == 0x08 && p_frag->i_buffer > 8 )
        {
            /* picture extension */
            p_sys->i_picture_structure = p_frag->p_buffer[6]&0x03;
            p_sys->i_top_field_first   = p_frag->p_buffer[7] >> 7;
            p_sys->i_repeat_first_field= (p_frag->p_buffer[7]>>1)&0x01;
            p_sys->i_progressive_frame = p_frag->p_buffer[8] >> 7;
        }
    }
    else if( p_frag->p_buffer[3] == 0xb2 && p_frag->i_buffer > 4 )
    {
        cc_Extract( &p_sys->cc, p_sys->i_top_field_first,
                    &p_frag->p_buffer[4], p_frag->i_buffer - 4 );
    }
    else if( p_frag->p_buffer[3] == 0x00 )
    {
        /* Picture start code */
        p_sys->i_seq_old++;
        if( p_frag->i_buffer >= 6 )
        {
            p_sys->i_temporal_ref =
                ( p_frag->p_buffer[4] << 2 )|(p_frag->p_buffer[5] >> 6);
            p_sys->i_picture_type = ( p_frag->p_buffer[5] >> 3 ) & 0x03;
        }
        p_sys->i_dts = p_frag->i_dts;
        p_sys->i_pts = p_frag->i_pts;
    }
    else if( p_frag->p_buffer[3] >= 0x01 && p_frag->p_buffer[3] <= 0xaf )
    {
        /* Slice start code */
        p_sys->b_frame_slice = true;
    }
    /* Append the block */
    block_ChainLastAppend( &p_sys->pp_last, p_frag );
    return p_pic;
}
