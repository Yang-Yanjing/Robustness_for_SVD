}
int input_DecoderSetCcState( decoder_t *p_dec, bool b_decode, int i_channel )
{
    decoder_owner_sys_t *p_owner = p_dec->p_owner;
    //msg_Warn( p_dec, "input_DecoderSetCcState: %d @%d", b_decode, i_channel );
    if( i_channel < 0 || i_channel >= 4 || !p_owner->cc.pb_present[i_channel] )
        return VLC_EGENERIC;
    if( b_decode )
    {
        static const vlc_fourcc_t fcc[4] = {
            VLC_FOURCC('c', 'c', '1', ' '),
            VLC_FOURCC('c', 'c', '2', ' '),
            VLC_FOURCC('c', 'c', '3', ' '),
            VLC_FOURCC('c', 'c', '4', ' '),
        };
        decoder_t *p_cc;
        es_format_t fmt;
        es_format_Init( &fmt, SPU_ES, fcc[i_channel] );
        p_cc = input_DecoderNew( p_owner->p_input, &fmt,
                              p_dec->p_owner->p_clock, p_owner->p_sout );
        if( !p_cc )
        {
            msg_Err( p_dec, "could not create decoder" );
            dialog_Fatal( p_dec, _("Streaming / Transcoding failed"), "%s",
                          _("VLC could not open the decoder module.") );
            return VLC_EGENERIC;
        }
        else if( !p_cc->p_module )
        {
            DecoderUnsupportedCodec( p_dec, fcc[i_channel] );
            input_DecoderDelete(p_cc);
            return VLC_EGENERIC;
        }
        p_cc->p_owner->p_clock = p_owner->p_clock;
        vlc_mutex_lock( &p_owner->lock );
        p_owner->cc.pp_decoder[i_channel] = p_cc;
        vlc_mutex_unlock( &p_owner->lock );
    }
    else
    {
        decoder_t *p_cc;
        vlc_mutex_lock( &p_owner->lock );
        p_cc = p_owner->cc.pp_decoder[i_channel];
        p_owner->cc.pp_decoder[i_channel] = NULL;
        vlc_mutex_unlock( &p_owner->lock );
        if( p_cc )
            input_DecoderDelete(p_cc);
    }
    return VLC_SUCCESS;
}
