}
int transcode_audio_new( sout_stream_t *p_stream,
                                sout_stream_id_sys_t *id )
{
    sout_stream_sys_t *p_sys = p_stream->p_sys;
    audio_sample_format_t fmt_last;
    /*
     * Open decoder
     */
    /* Initialization of decoder structures */
    id->p_decoder->fmt_out = id->p_decoder->fmt_in;
    id->p_decoder->fmt_out.i_extra = 0;
    id->p_decoder->fmt_out.p_extra = 0;
    id->p_decoder->pf_decode_audio = NULL;
    id->p_decoder->pf_aout_format_update = audio_update_format;
    /* id->p_decoder->p_cfg = p_sys->p_audio_cfg; */
    id->p_decoder->p_module =
        module_need( id->p_decoder, "decoder", "$codec", false );
    if( !id->p_decoder->p_module )
    {
        msg_Err( p_stream, "cannot find audio decoder" );
        return VLC_EGENERIC;
    }
    /* decoders don't set audio.i_format, but audio filters use it */
    id->p_decoder->fmt_out.audio.i_format = id->p_decoder->fmt_out.i_codec;
    aout_FormatPrepare( &id->p_decoder->fmt_out.audio );
    fmt_last = id->p_decoder->fmt_out.audio;
    /* Fix AAC SBR changing number of channels and sampling rate */
    if( !(id->p_decoder->fmt_in.i_codec == VLC_CODEC_MP4A &&
        fmt_last.i_rate != id->p_encoder->fmt_in.audio.i_rate &&
        fmt_last.i_channels != id->p_encoder->fmt_in.audio.i_channels) )
        fmt_last.i_rate = id->p_decoder->fmt_in.audio.i_rate;
    /*
     * Open encoder
     */
    if( transcode_audio_initialize_encoder( id, p_stream ) == VLC_EGENERIC )
        return VLC_EGENERIC;
    if( unlikely( transcode_audio_initialize_filters( p_stream, id, p_sys,
                                                      &fmt_last ) != VLC_SUCCESS ) )
        return VLC_EGENERIC;
    return VLC_SUCCESS;
}
