}
static int transcode_audio_initialize_filters( sout_stream_t *p_stream, sout_stream_id_sys_t *id,
                                               sout_stream_sys_t *p_sys, audio_sample_format_t *fmt_last )
{
    /* Load user specified audio filters */
    /* XXX: These variable names come kinda out of nowhere... */
    var_Create( p_stream, "audio-time-stretch", VLC_VAR_BOOL );
    var_Create( p_stream, "audio-filter", VLC_VAR_STRING );
    if( p_sys->psz_af )
        var_SetString( p_stream, "audio-filter", p_sys->psz_af );
    id->p_af_chain = aout_FiltersNew( p_stream, fmt_last,
                                      &id->p_encoder->fmt_in.audio, NULL );
    var_Destroy( p_stream, "audio-filter" );
    var_Destroy( p_stream, "audio-time-stretch" );
    if( id->p_af_chain == NULL )
    {
        msg_Err( p_stream, "Unable to initialize audio filters" );
        module_unneed( id->p_encoder, id->p_encoder->p_module );
        id->p_encoder->p_module = NULL;
        module_unneed( id->p_decoder, id->p_decoder->p_module );
        id->p_decoder->p_module = NULL;
        return VLC_EGENERIC;
    }
    id->fmt_audio.i_rate = fmt_last->i_rate;
    id->fmt_audio.i_physical_channels = fmt_last->i_physical_channels;
    return VLC_SUCCESS;
}
