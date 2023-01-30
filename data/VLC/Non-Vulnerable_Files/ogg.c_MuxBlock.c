}
static int MuxBlock( sout_mux_t *p_mux, sout_input_t *p_input )
{
    sout_mux_sys_t *p_sys = p_mux->p_sys;
    ogg_stream_t *p_stream = (ogg_stream_t*)p_input->p_sys;
    block_t *p_data = block_FifoGet( p_input->p_fifo );
    block_t *p_og = NULL;
    ogg_packet op;
    uint64_t i_time;
    if( p_stream->i_fourcc != VLC_CODEC_VORBIS &&
        p_stream->i_fourcc != VLC_CODEC_FLAC &&
        p_stream->i_fourcc != VLC_CODEC_SPEEX &&
        p_stream->i_fourcc != VLC_CODEC_OPUS &&
        p_stream->i_fourcc != VLC_CODEC_THEORA &&
        p_stream->i_fourcc != VLC_CODEC_VP8 &&
        p_stream->i_fourcc != VLC_CODEC_DIRAC )
    {
        p_data = block_Realloc( p_data, 1, p_data->i_buffer );
        p_data->p_buffer[0] = PACKET_IS_SYNCPOINT;      // FIXME
    }
    if ( p_stream->i_fourcc == VLC_CODEC_DIRAC && p_stream->i_baseptsdelay < 0 )
        p_stream->i_baseptsdelay = p_data->i_pts - p_data->i_dts;
    op.packet   = p_data->p_buffer;
    op.bytes    = p_data->i_buffer;
    op.b_o_s    = 0;
    op.e_o_s    = 0;
    op.packetno = p_stream->i_packet_no++;
    op.granulepos = -1;
    if( p_stream->i_cat == AUDIO_ES )
    {
        if( p_stream->i_fourcc == VLC_CODEC_VORBIS ||
            p_stream->i_fourcc == VLC_CODEC_FLAC ||
            p_stream->i_fourcc == VLC_CODEC_OPUS ||
            p_stream->i_fourcc == VLC_CODEC_SPEEX )
        {
            /* number of sample from begining + current packet */
            op.granulepos =
                ( p_data->i_dts - p_sys->i_start_dts + p_data->i_length ) *
                (mtime_t)p_input->p_fmt->audio.i_rate / CLOCK_FREQ;
            i_time = p_data->i_dts - p_sys->i_start_dts;
            AddIndexEntry( p_mux, i_time, p_input );
        }
        else if( p_stream->p_oggds_header )
        {
            /* number of sample from begining */
            op.granulepos = ( p_data->i_dts - p_sys->i_start_dts ) *
                p_stream->p_oggds_header->i_samples_per_unit / CLOCK_FREQ;
        }
    }
    else if( p_stream->i_cat == VIDEO_ES )
    {
        if( p_stream->i_fourcc == VLC_CODEC_THEORA )
        {
            p_stream->i_num_frames++;
            if( p_data->i_flags & BLOCK_FLAG_TYPE_I )
            {
                p_stream->i_num_keyframes++;
                p_stream->i_last_keyframe = p_stream->i_num_frames;
                /* presentation time */
                i_time = CLOCK_FREQ * ( p_stream->i_num_frames - 1 ) *
                         p_input->p_fmt->video.i_frame_rate_base /  p_input->p_fmt->video.i_frame_rate;
                AddIndexEntry( p_mux, i_time, p_input );
            }
            op.granulepos = (p_stream->i_last_keyframe << p_stream->i_keyframe_granule_shift )
                          | (p_stream->i_num_frames-p_stream->i_last_keyframe);
        }
        else if( p_stream->i_fourcc == VLC_CODEC_DIRAC )
        {
#define FRAME_ROUND(a) \
    if ( ( a + 5000 / CLOCK_FREQ ) > ( a / CLOCK_FREQ ) )\
        a += 5000;\
    a /= CLOCK_FREQ;
            mtime_t dt = (p_data->i_dts - p_sys->i_start_dts) * p_input->p_fmt->video.i_frame_rate / p_input->p_fmt->video.i_frame_rate_base;
            FRAME_ROUND( dt );
            mtime_t pt = (p_data->i_pts - p_sys->i_start_dts - p_stream->i_baseptsdelay ) * p_input->p_fmt->video.i_frame_rate / p_input->p_fmt->video.i_frame_rate_base;
            FRAME_ROUND( pt );
            /* (shro) some PTS could be repeated within 1st frames */
            if ( pt == p_stream->i_dirac_last_pt )
                pt++;
            else
                p_stream->i_dirac_last_pt = pt;
            /* (shro) some DTS could be repeated within 1st frames */
            if ( dt == p_stream->i_dirac_last_dt )
                dt++;
            else
                p_stream->i_dirac_last_dt = dt;
            if( p_data->i_flags & BLOCK_FLAG_TYPE_I )
                p_stream->i_last_keyframe = dt;
            mtime_t dist = dt - p_stream->i_last_keyframe;
            /* Everything increments by two for progressive */
            if ( true )
            {
                pt *=2;
                dt *=2;
            }
            mtime_t delay = pt - dt;
            if ( delay < 0 ) delay *= -1;
            op.granulepos = (pt - delay) << 31 | (dist&0xff00) << 14
                          | (delay&0x1fff) << 9 | (dist&0xff);
#ifndef NDEBUG
            msg_Dbg( p_mux, "dts %"PRId64" pts %"PRId64" dt %"PRId64" pt %"PRId64" delay %"PRId64" granule %"PRId64,
                     (p_data->i_dts - p_sys->i_start_dts),
                     (p_data->i_pts - p_sys->i_start_dts ),
                     dt, pt, delay, op.granulepos );
#endif
            AddIndexEntry( p_mux, dt, p_input );
        }
        else if( p_stream->i_fourcc == VLC_CODEC_VP8 )
        {
            p_stream->i_num_frames++;
            if( p_data->i_flags & BLOCK_FLAG_TYPE_I )
            {
                p_stream->i_num_keyframes++;
                p_stream->i_last_keyframe = p_stream->i_num_frames;
                /* presentation time */
                i_time = CLOCK_FREQ * ( p_stream->i_num_frames - 1 ) *
                         p_input->p_fmt->video.i_frame_rate_base /  p_input->p_fmt->video.i_frame_rate;
                AddIndexEntry( p_mux, i_time, p_input );
            }
            op.granulepos = ( ((int64_t)p_stream->i_num_frames) << 32 ) |
            ( ( ( p_stream->i_num_frames - p_stream->i_last_keyframe ) & 0x07FFFFFF ) << 3 );
        }
        else if( p_stream->p_oggds_header )
            op.granulepos = ( p_data->i_dts - p_sys->i_start_dts ) * INT64_C(10) /
                p_stream->p_oggds_header->i_time_unit;
    }
    else if( p_stream->i_cat == SPU_ES )
    {
        /* granulepos is in millisec */
        op.granulepos = ( p_data->i_dts - p_sys->i_start_dts ) / 1000;
    }
    else
        return VLC_EGENERIC;
    p_stream->u_last_granulepos = op.granulepos;
    ogg_stream_packetin( &p_stream->os, &op );
    if( p_stream->i_cat == SPU_ES ||
        p_stream->i_fourcc == VLC_CODEC_SPEEX ||
        p_stream->i_fourcc == VLC_CODEC_DIRAC )
    {
        /* Subtitles or Speex packets are quite small so they
         * need to be flushed to be sent on time */
        /* The OggDirac mapping suggests ever so strongly that a
         * page flush occurs after each OggDirac packet, so to make
         * the timestamps unambiguous */
        p_og = OggStreamFlush( p_mux, &p_stream->os, p_data->i_dts );
    }
    else
    {
        p_og = OggStreamPageOut( p_mux, &p_stream->os, p_data->i_dts );
    }
    if( p_og )
    {
        OggSetDate( p_og, p_stream->i_dts, p_stream->i_length );
        p_stream->i_dts = -1;
        p_stream->i_length = 0;
        p_mux->p_sys->i_pos += sout_AccessOutWrite( p_mux->p_access, p_og );
    }
    else
    {
        if( p_stream->i_dts < 0 )
        {
            p_stream->i_dts = p_data->i_dts;
        }
        p_stream->i_length += p_data->i_length;
    }
    block_Release( p_data );
    return VLC_SUCCESS;
}
