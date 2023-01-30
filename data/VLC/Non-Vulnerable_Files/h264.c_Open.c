 *****************************************************************************/
static int Open( vlc_object_t *p_this )
{
    decoder_t     *p_dec = (decoder_t*)p_this;
    decoder_sys_t *p_sys;
    int i;
    if( p_dec->fmt_in.i_codec != VLC_CODEC_H264 )
        return VLC_EGENERIC;
    if( p_dec->fmt_in.i_original_fourcc == VLC_FOURCC( 'a', 'v', 'c', '1') &&
        p_dec->fmt_in.i_extra < 7 )
        return VLC_EGENERIC;
    /* Allocate the memory needed to store the decoder's structure */
    if( ( p_dec->p_sys = p_sys = malloc( sizeof(decoder_sys_t) ) ) == NULL )
    {
        return VLC_ENOMEM;
    }
    packetizer_Init( &p_sys->packetizer,
                     p_h264_startcode, sizeof(p_h264_startcode),
                     p_h264_startcode, 1, 5,
                     PacketizeReset, PacketizeParse, PacketizeValidate, p_dec );
    p_sys->b_slice = false;
    p_sys->p_frame = NULL;
    p_sys->b_frame_sps = false;
    p_sys->b_frame_pps = false;
    p_sys->b_header= false;
    p_sys->b_sps   = false;
    p_sys->b_pps   = false;
    for( i = 0; i < SPS_MAX; i++ )
        p_sys->pp_sps[i] = NULL;
    for( i = 0; i < PPS_MAX; i++ )
        p_sys->pp_pps[i] = NULL;
    p_sys->i_recovery_frames = -1;
    p_sys->slice.i_nal_type = -1;
    p_sys->slice.i_nal_ref_idc = -1;
    p_sys->slice.i_idr_pic_id = -1;
    p_sys->slice.i_frame_num = -1;
    p_sys->slice.i_frame_type = 0;
    p_sys->slice.i_pic_parameter_set_id = -1;
    p_sys->slice.i_field_pic_flag = 0;
    p_sys->slice.i_bottom_field_flag = -1;
    p_sys->slice.i_pic_order_cnt_lsb = -1;
    p_sys->slice.i_delta_pic_order_cnt_bottom = -1;
    p_sys->i_frame_dts = VLC_TS_INVALID;
    p_sys->i_frame_pts = VLC_TS_INVALID;
    /* Setup properties */
    es_format_Copy( &p_dec->fmt_out, &p_dec->fmt_in );
    p_dec->fmt_out.i_codec = VLC_CODEC_H264;
    if( p_dec->fmt_in.i_original_fourcc == VLC_FOURCC( 'a', 'v', 'c', '1' ) )
    {
        /* This type of stream is produced by mp4 and matroska
         * when we want to store it in another streamformat, you need to convert
         * The fmt_in.p_extra should ALWAYS contain the avcC
         * The fmt_out.p_extra should contain all the SPS and PPS with 4 byte startcodes */
        uint8_t *p = &((uint8_t*)p_dec->fmt_in.p_extra)[4];
        int i_sps, i_pps;
        bool b_dummy;
        int i;
        /* Parse avcC */
        p_sys->i_avcC_length_size = 1 + ((*p++)&0x03);
        /* Read SPS */
        i_sps = (*p++)&0x1f;
        for( i = 0; i < i_sps; i++ )
        {
            uint16_t i_length = GetWBE( p ); p += 2;
            if( i_length >
                (uint8_t*)p_dec->fmt_in.p_extra + p_dec->fmt_in.i_extra - p )
            {
                return VLC_EGENERIC;
            }
            block_t *p_sps = CreateAnnexbNAL( p_dec, p, i_length );
            if( !p_sps )
                return VLC_EGENERIC;
            ParseNALBlock( p_dec, &b_dummy, p_sps );
            p += i_length;
        }
        /* Read PPS */
        i_pps = *p++;
        for( i = 0; i < i_pps; i++ )
        {
            uint16_t i_length = GetWBE( p ); p += 2;
            if( i_length >
                (uint8_t*)p_dec->fmt_in.p_extra + p_dec->fmt_in.i_extra - p )
            {
                return VLC_EGENERIC;
            }
            block_t *p_pps = CreateAnnexbNAL( p_dec, p, i_length );
            if( !p_pps )
                return VLC_EGENERIC;
            ParseNALBlock( p_dec, &b_dummy, p_pps );
            p += i_length;
        }
        msg_Dbg( p_dec, "avcC length size=%d, sps=%d, pps=%d",
                 p_sys->i_avcC_length_size, i_sps, i_pps );
        if( !p_sys->b_sps || !p_sys->b_pps )
            return VLC_EGENERIC;
        /* FIXME: FFMPEG isn't happy at all if you leave this */
        if( p_dec->fmt_out.i_extra > 0 )
            free( p_dec->fmt_out.p_extra );
        p_dec->fmt_out.i_extra = 0;
        p_dec->fmt_out.p_extra = NULL;
        /* Set the new extradata */
        for( i = 0; i < SPS_MAX; i++ )
        {
            if( p_sys->pp_sps[i] )
                p_dec->fmt_out.i_extra += p_sys->pp_sps[i]->i_buffer;
        }
        for( i = 0; i < PPS_MAX; i++ )
        {
            if( p_sys->pp_pps[i] )
                p_dec->fmt_out.i_extra += p_sys->pp_pps[i]->i_buffer;
        }
        p_dec->fmt_out.p_extra = malloc( p_dec->fmt_out.i_extra );
        if( p_dec->fmt_out.p_extra )
        {
            uint8_t *p_dst = p_dec->fmt_out.p_extra;
            for( i = 0; i < SPS_MAX; i++ )
            {
                if( p_sys->pp_sps[i] )
                {
                    memcpy( p_dst, p_sys->pp_sps[i]->p_buffer, p_sys->pp_sps[i]->i_buffer );
                    p_dst += p_sys->pp_sps[i]->i_buffer;
                }
            }
            for( i = 0; i < PPS_MAX; i++ )
            {
                if( p_sys->pp_pps[i] )
                {
                    memcpy( p_dst, p_sys->pp_pps[i]->p_buffer, p_sys->pp_pps[i]->i_buffer );
                    p_dst += p_sys->pp_pps[i]->i_buffer;
                }
            }
            p_sys->b_header = true;
        }
        else
        {
            p_dec->fmt_out.i_extra = 0;
        }
        /* Set callback */
        p_dec->pf_packetize = PacketizeAVC1;
        /* TODO CC ? */
    }
    else
    {
        /* This type of stream contains data with 3 of 4 byte startcodes
         * The fmt_in.p_extra MAY contain SPS/PPS with 4 byte startcodes
         * The fmt_out.p_extra should be the same */
        /* Set callback */
        p_dec->pf_packetize = Packetize;
        p_dec->pf_get_cc = GetCc;
        /* */
        p_sys->i_cc_pts = VLC_TS_INVALID;
        p_sys->i_cc_dts = VLC_TS_INVALID;
        p_sys->i_cc_flags = 0;
        cc_Init( &p_sys->cc );
        cc_Init( &p_sys->cc_next );
        /* */
        if( p_dec->fmt_in.i_extra > 0 )
            packetizer_Header( &p_sys->packetizer,
                               p_dec->fmt_in.p_extra, p_dec->fmt_in.i_extra );
    }
    return VLC_SUCCESS;
}
