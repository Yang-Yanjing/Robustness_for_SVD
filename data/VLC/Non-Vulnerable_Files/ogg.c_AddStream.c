 *****************************************************************************/
static int AddStream( sout_mux_t *p_mux, sout_input_t *p_input )
{
    sout_mux_sys_t *p_sys = p_mux->p_sys;
    ogg_stream_t   *p_stream;
    uint16_t i_tag;
    msg_Dbg( p_mux, "adding input" );
    p_input->p_sys = p_stream = calloc( 1, sizeof( ogg_stream_t ) );
    if( !p_stream )
        return VLC_ENOMEM;
    p_stream->i_cat       = p_input->p_fmt->i_cat;
    p_stream->i_fourcc    = p_input->p_fmt->i_codec;
    p_stream->i_serial_no = p_sys->i_next_serial_no++;
    p_stream->i_packet_no = 0;
    p_stream->i_last_keyframe = 0;
    p_stream->i_num_keyframes = 0;
    p_stream->i_num_frames = 0;
    p_stream->p_oggds_header = 0;
    p_stream->i_baseptsdelay = -1;
    p_stream->i_dirac_last_pt = -1;
    p_stream->i_dirac_last_dt = -1;
    switch( p_input->p_fmt->i_cat )
    {
    case VIDEO_ES:
        if( !p_input->p_fmt->video.i_frame_rate ||
            !p_input->p_fmt->video.i_frame_rate_base )
        {
            msg_Warn( p_mux, "Missing frame rate, assuming 25fps" );
            p_input->p_fmt->video.i_frame_rate = 25;
            p_input->p_fmt->video.i_frame_rate_base = 1;
        }
        switch( p_stream->i_fourcc )
        {
        case VLC_CODEC_MP4V:
        case VLC_CODEC_MPGV:
        case VLC_CODEC_MP1V:
        case VLC_CODEC_MP2V:
        case VLC_CODEC_DIV3:
        case VLC_CODEC_MJPG:
        case VLC_CODEC_WMV1:
        case VLC_CODEC_WMV2:
        case VLC_CODEC_WMV3:
            p_stream->p_oggds_header = calloc( 1, sizeof(oggds_header_t) );
            if( !p_stream->p_oggds_header )
            {
                free( p_stream );
                return VLC_ENOMEM;
            }
            p_stream->p_oggds_header->i_packet_type = PACKET_TYPE_HEADER;
            memcpy( p_stream->p_oggds_header->stream_type, "video", 5 );
            if( p_stream->i_fourcc == VLC_CODEC_MP4V )
            {
                memcpy( p_stream->p_oggds_header->sub_type, "XVID", 4 );
            }
            else if( p_stream->i_fourcc == VLC_CODEC_DIV3 )
            {
                memcpy( p_stream->p_oggds_header->sub_type, "DIV3", 4 );
            }
            else
            {
                memcpy( p_stream->p_oggds_header->sub_type,
                        &p_stream->i_fourcc, 4 );
            }
            p_stream->p_oggds_header->i_size = 0 ;
            p_stream->p_oggds_header->i_time_unit =
                     INT64_C(10000000) * p_input->p_fmt->video.i_frame_rate_base /
                     (int64_t)p_input->p_fmt->video.i_frame_rate;
            p_stream->p_oggds_header->i_samples_per_unit = 1;
            p_stream->p_oggds_header->i_default_len = 1 ; /* ??? */
            p_stream->p_oggds_header->i_buffer_size = 1024*1024;
            p_stream->p_oggds_header->i_bits_per_sample = 0;
            p_stream->p_oggds_header->header.video.i_width = p_input->p_fmt->video.i_width;
            p_stream->p_oggds_header->header.video.i_height = p_input->p_fmt->video.i_height;
            msg_Dbg( p_mux, "%4.4s stream", (char *)&p_stream->i_fourcc );
            break;
        case VLC_CODEC_DIRAC:
            msg_Dbg( p_mux, "dirac stream" );
            break;
        case VLC_CODEC_THEORA:
            msg_Dbg( p_mux, "theora stream" );
            break;
        case VLC_CODEC_VP8:
            msg_Dbg( p_mux, "VP8 stream" );
            break;
        default:
            FREENULL( p_input->p_sys );
            return VLC_EGENERIC;
        }
        break;
    case AUDIO_ES:
        switch( p_stream->i_fourcc )
        {
        case VLC_CODEC_OPUS:
            msg_Dbg( p_mux, "opus stream" );
            break;
        case VLC_CODEC_VORBIS:
            msg_Dbg( p_mux, "vorbis stream" );
            break;
        case VLC_CODEC_SPEEX:
            msg_Dbg( p_mux, "speex stream" );
            break;
        case VLC_CODEC_FLAC:
            msg_Dbg( p_mux, "flac stream" );
            break;
        default:
            fourcc_to_wf_tag( p_stream->i_fourcc, &i_tag );
            if( i_tag == WAVE_FORMAT_UNKNOWN )
            {
                FREENULL( p_input->p_sys );
                return VLC_EGENERIC;
            }
            p_stream->p_oggds_header =
                malloc( sizeof(oggds_header_t) + p_input->p_fmt->i_extra );
            if( !p_stream->p_oggds_header )
            {
                free( p_stream );
                return VLC_ENOMEM;
            }
            memset( p_stream->p_oggds_header, 0, sizeof(oggds_header_t) );
            p_stream->p_oggds_header->i_packet_type = PACKET_TYPE_HEADER;
            p_stream->p_oggds_header->i_size = p_input->p_fmt->i_extra;
            if( p_input->p_fmt->i_extra )
            {
                memcpy( &p_stream->p_oggds_header[1],
                        p_input->p_fmt->p_extra, p_input->p_fmt->i_extra );
            }
            memcpy( p_stream->p_oggds_header->stream_type, "audio", 5 );
            memset( p_stream->p_oggds_header->sub_type, 0, 4 );
            char buf[5];
            snprintf( buf, sizeof(buf), "%"PRIx16, i_tag );
            strncpy( p_stream->p_oggds_header->sub_type, buf, 4 );
            p_stream->p_oggds_header->i_time_unit = INT64_C(10000000);
            p_stream->p_oggds_header->i_default_len = 1;
            p_stream->p_oggds_header->i_buffer_size = 30*1024 ;
            p_stream->p_oggds_header->i_samples_per_unit = p_input->p_fmt->audio.i_rate;
            p_stream->p_oggds_header->i_bits_per_sample = p_input->p_fmt->audio.i_bitspersample;
            p_stream->p_oggds_header->header.audio.i_channels = p_input->p_fmt->audio.i_channels;
            p_stream->p_oggds_header->header.audio.i_block_align =  p_input->p_fmt->audio.i_blockalign;
            p_stream->p_oggds_header->header.audio.i_avgbytespersec =  p_input->p_fmt->i_bitrate / 8;
            msg_Dbg( p_mux, "%4.4s stream", (char *)&p_stream->i_fourcc );
            break;
        }
        break;
    case SPU_ES:
        switch( p_stream->i_fourcc )
        {
        case VLC_CODEC_SUBT:
            p_stream->p_oggds_header = calloc( 1, sizeof(oggds_header_t) );
            if( !p_stream->p_oggds_header )
            {
                free( p_stream );
                return VLC_ENOMEM;
            }
            p_stream->p_oggds_header->i_packet_type = PACKET_TYPE_HEADER;
            memcpy( p_stream->p_oggds_header->stream_type, "text", 4 );
            msg_Dbg( p_mux, "subtitles stream" );
            break;
        default:
            FREENULL( p_input->p_sys );
            return VLC_EGENERIC;
        }
        break;
    default:
        FREENULL( p_input->p_sys );
        return VLC_EGENERIC;
    }
    p_stream->b_new = true;
    p_sys->i_add_streams++;
    return VLC_SUCCESS;
}
