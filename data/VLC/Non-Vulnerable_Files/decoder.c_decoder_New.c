/* TODO: pass p_sout through p_resource? -- Courmisch */
static decoder_t *decoder_New( vlc_object_t *p_parent, input_thread_t *p_input,
                               es_format_t *fmt, input_clock_t *p_clock,
                               input_resource_t *p_resource,
                               sout_instance_t *p_sout  )
{
    decoder_t *p_dec = NULL;
    const char *psz_type = p_sout ? N_("packetizer") : N_("decoder");
    int i_priority;
    /* Create the decoder configuration structure */
    p_dec = CreateDecoder( p_parent, p_input, fmt,
                           p_sout != NULL, p_resource, p_sout );
    if( p_dec == NULL )
    {
        msg_Err( p_parent, "could not create %s", psz_type );
        dialog_Fatal( p_parent, _("Streaming / Transcoding failed"),
                      _("VLC could not open the %s module."),
                      vlc_gettext( psz_type ) );
        return NULL;
    }
    if( !p_dec->p_module )
    {
        DecoderUnsupportedCodec( p_dec, fmt->i_codec );
        DeleteDecoder( p_dec );
        return NULL;
    }
    p_dec->p_owner->p_clock = p_clock;
    assert( p_dec->fmt_out.i_cat != UNKNOWN_ES );
    if( p_dec->fmt_out.i_cat == AUDIO_ES )
        i_priority = VLC_THREAD_PRIORITY_AUDIO;
    else
        i_priority = VLC_THREAD_PRIORITY_VIDEO;
    /* Spawn the decoder thread */
    if( vlc_clone( &p_dec->p_owner->thread, DecoderThread, p_dec, i_priority ) )
    {
        msg_Err( p_dec, "cannot spawn decoder thread" );
        module_unneed( p_dec, p_dec->p_module );
        DeleteDecoder( p_dec );
        return NULL;
    }
    return p_dec;
}
