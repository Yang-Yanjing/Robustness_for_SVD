 *****************************************************************************/
static int AddStream( sout_mux_t *p_mux, sout_input_t *p_input )
{
    sout_mux_sys_t *p_sys = p_mux->p_sys;
    es_format_t *fmt = p_input->p_fmt;
    AVCodecContext *codec;
    AVStream *stream;
    unsigned i_codec_id;
    msg_Dbg( p_mux, "adding input" );
    if( !GetFfmpegCodec( fmt->i_codec, 0, &i_codec_id, 0 )
     || i_codec_id == AV_CODEC_ID_NONE )
    {
        msg_Dbg( p_mux, "couldn't find codec for fourcc '%4.4s'",
                 (char *)&fmt->i_codec );
        return VLC_EGENERIC;
    }
    unsigned opus_size[XIPH_MAX_HEADER_COUNT];
    void     *opus_packet[XIPH_MAX_HEADER_COUNT];
    if( fmt->i_codec == VLC_CODEC_OPUS )
    {
        unsigned count;
        /* Only transmits the first packet (OpusHead) */
        if( xiph_SplitHeaders(opus_size, opus_packet, &count, fmt->i_extra, fmt->p_extra ) ) {
            count = 0;
        }
        if (count != 2 || opus_size[0] < 19) {
            msg_Err(p_mux, "Invalid Opus header");
            return VLC_EGENERIC;
        }
    }
    p_input->p_sys = malloc( sizeof( int ) );
    *((int *)p_input->p_sys) = p_sys->oc->nb_streams;
    if( fmt->i_cat != VIDEO_ES && fmt->i_cat != AUDIO_ES)
    {
        msg_Warn( p_mux, "Unhandled ES category" );
        return VLC_EGENERIC;
    }
    stream = avformat_new_stream( p_sys->oc, NULL);
    if( !stream )
    {
        free( p_input->p_sys );
        return VLC_EGENERIC;
    }
    codec = stream->codec;
    codec->opaque = p_mux;
    switch( fmt->i_cat )
    {
    case AUDIO_ES:
        codec->codec_type = AVMEDIA_TYPE_AUDIO;
        codec->channels = fmt->audio.i_channels;
        codec->sample_rate = fmt->audio.i_rate;
        stream->time_base = (AVRational){1, codec->sample_rate};
        codec->frame_size = fmt->audio.i_frame_length;
        if (fmt->i_bitrate == 0) {
            msg_Warn( p_mux, "Missing audio bitrate, assuming 64k" );
            fmt->i_bitrate = 64000;
        }
        break;
    case VIDEO_ES:
        if( !fmt->video.i_frame_rate || !fmt->video.i_frame_rate_base ) {
            msg_Warn( p_mux, "Missing frame rate, assuming 25fps" );
            fmt->video.i_frame_rate = 25;
            fmt->video.i_frame_rate_base = 1;
        } else
            msg_Dbg( p_mux, "Muxing framerate will be %d/%d = %.2f fps",
                    fmt->video.i_frame_rate,
                    fmt->video.i_frame_rate_base,
                    (double)fmt->video.i_frame_rate/(double)fmt->video.i_frame_rate_base );
        codec->codec_type = AVMEDIA_TYPE_VIDEO;
        codec->width = fmt->video.i_visible_width;
        codec->height = fmt->video.i_visible_height;
        av_reduce( &codec->sample_aspect_ratio.num,
                   &codec->sample_aspect_ratio.den,
                   fmt->video.i_sar_num,
                   fmt->video.i_sar_den, 1 << 30 /* something big */ );
        msg_Dbg(p_mux, "Muxing aspect ratio will be %d/%d",
                fmt->video.i_sar_num, fmt->video.i_sar_den);
        stream->sample_aspect_ratio.den = codec->sample_aspect_ratio.den;
        stream->sample_aspect_ratio.num = codec->sample_aspect_ratio.num;
        stream->time_base.den = fmt->video.i_frame_rate;
        stream->time_base.num = fmt->video.i_frame_rate_base;
        if (fmt->i_bitrate == 0) {
            msg_Warn( p_mux, "Missing video bitrate, assuming 512k" );
            fmt->i_bitrate = 512000;
        } else
            msg_Dbg( p_mux, "Muxing video bitrate will be %d", fmt->i_bitrate );
        break;
    }
    codec->bit_rate = fmt->i_bitrate;
    codec->codec_tag = av_codec_get_tag( p_sys->oc->oformat->codec_tag, i_codec_id );
    if( !codec->codec_tag && i_codec_id == AV_CODEC_ID_MP2 )
    {
        i_codec_id = AV_CODEC_ID_MP3;
        codec->codec_tag = av_codec_get_tag( p_sys->oc->oformat->codec_tag, i_codec_id );
    }
    codec->codec_id = i_codec_id;
    if( fmt->i_extra )
    {
        if( fmt->i_codec == VLC_CODEC_OPUS )
        {
            codec->extradata_size = opus_size[0];
            codec->extradata = av_malloc( opus_size[0] );
            memcpy( codec->extradata, opus_packet[0], opus_size[0] );
        }
        else
        {
            codec->extradata_size = fmt->i_extra;
            codec->extradata = av_malloc( fmt->i_extra );
            memcpy( codec->extradata, fmt->p_extra, fmt->i_extra );
        }
    }
    return VLC_SUCCESS;
}
