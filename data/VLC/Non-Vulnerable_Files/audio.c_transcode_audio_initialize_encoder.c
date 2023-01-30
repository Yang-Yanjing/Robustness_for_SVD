}
static int transcode_audio_initialize_encoder( sout_stream_id_sys_t *id, sout_stream_t *p_stream )
{
    sout_stream_sys_t *p_sys = p_stream->p_sys;
    /* Initialization of encoder format structures */
    es_format_Init( &id->p_encoder->fmt_in, id->p_decoder->fmt_in.i_cat,
                    id->p_decoder->fmt_out.i_codec );
    id->p_encoder->fmt_in.audio.i_format = id->p_decoder->fmt_out.i_codec;
    id->p_encoder->fmt_in.audio.i_rate = id->p_encoder->fmt_out.audio.i_rate;
    id->p_encoder->fmt_in.audio.i_physical_channels =
        id->p_encoder->fmt_out.audio.i_physical_channels;
    aout_FormatPrepare( &id->p_encoder->fmt_in.audio );
    id->p_encoder->p_cfg = p_stream->p_sys->p_audio_cfg;
    id->p_encoder->p_module =
        module_need( id->p_encoder, "encoder", p_sys->psz_aenc, true );
    /* p_sys->i_acodec = 0 if there isn't acodec defined */
    if( !id->p_encoder->p_module && p_sys->i_acodec )
    {
        msg_Err( p_stream, "cannot find audio encoder (module:%s fourcc:%4.4s). "
                           "Take a look few lines earlier to see possible reason.",
                 p_sys->psz_aenc ? p_sys->psz_aenc : "any",
                 (char *)&p_sys->i_acodec );
        module_unneed( id->p_decoder, id->p_decoder->p_module );
        id->p_decoder->p_module = NULL;
        return VLC_EGENERIC;
    }
    id->p_encoder->fmt_out.i_codec =
        vlc_fourcc_GetCodec( AUDIO_ES, id->p_encoder->fmt_out.i_codec );
    /* Fix input format */
    id->p_encoder->fmt_in.audio.i_format = id->p_encoder->fmt_in.i_codec;
    if( !id->p_encoder->fmt_in.audio.i_physical_channels
     || !id->p_encoder->fmt_in.audio.i_original_channels )
    {
        if( id->p_encoder->fmt_in.audio.i_channels < (sizeof(pi_channels_maps) / sizeof(*pi_channels_maps)) )
            id->p_encoder->fmt_in.audio.i_physical_channels =
            id->p_encoder->fmt_in.audio.i_original_channels =
                      pi_channels_maps[id->p_encoder->fmt_in.audio.i_channels];
    }
    aout_FormatPrepare( &id->p_encoder->fmt_in.audio );
    return VLC_SUCCESS;
}
