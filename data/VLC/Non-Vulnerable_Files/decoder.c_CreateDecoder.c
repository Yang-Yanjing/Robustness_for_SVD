 */
static decoder_t * CreateDecoder( vlc_object_t *p_parent,
                                  input_thread_t *p_input,
                                  es_format_t *fmt, bool b_packetizer,
                                  input_resource_t *p_resource,
                                  sout_instance_t *p_sout )
{
    decoder_t *p_dec;
    decoder_owner_sys_t *p_owner;
    es_format_t null_es_format;
    p_dec = vlc_custom_create( p_parent, sizeof( *p_dec ), "decoder" );
    if( p_dec == NULL )
        return NULL;
    p_dec->pf_decode_audio = NULL;
    p_dec->pf_decode_video = NULL;
    p_dec->pf_decode_sub = NULL;
    p_dec->pf_get_cc = NULL;
    p_dec->pf_packetize = NULL;
    /* Initialize the decoder */
    p_dec->p_module = NULL;
    memset( &null_es_format, 0, sizeof(es_format_t) );
    es_format_Copy( &p_dec->fmt_in, fmt );
    es_format_Copy( &p_dec->fmt_out, &null_es_format );
    p_dec->p_description = NULL;
    /* Allocate our private structure for the decoder */
    p_dec->p_owner = p_owner = malloc( sizeof( decoder_owner_sys_t ) );
    if( unlikely(p_owner == NULL) )
    {
        vlc_object_release( p_dec );
        return NULL;
    }
    p_owner->i_preroll_end = VLC_TS_INVALID;
    p_owner->i_last_rate = INPUT_RATE_DEFAULT;
    p_owner->p_input = p_input;
    p_owner->p_resource = p_resource;
    p_owner->p_aout = NULL;
    p_owner->p_vout = NULL;
    p_owner->p_spu_vout = NULL;
    p_owner->i_spu_channel = 0;
    p_owner->i_spu_order = 0;
    p_owner->p_sout = p_sout;
    p_owner->p_sout_input = NULL;
    p_owner->p_packetizer = NULL;
    p_owner->b_packetizer = b_packetizer;
    /* decoder fifo */
    p_owner->p_fifo = block_FifoNew();
    if( unlikely(p_owner->p_fifo == NULL) )
    {
        free( p_owner );
        vlc_object_release( p_dec );
        return NULL;
    }
    /* Set buffers allocation callbacks for the decoders */
    p_dec->pf_aout_format_update = aout_update_format;
    p_dec->pf_vout_buffer_new = vout_new_buffer;
    p_dec->pf_vout_buffer_del = vout_del_buffer;
    p_dec->pf_picture_link    = vout_link_picture;
    p_dec->pf_picture_unlink  = vout_unlink_picture;
    p_dec->pf_spu_buffer_new  = spu_new_buffer;
    p_dec->pf_spu_buffer_del  = spu_del_buffer;
    /* */
    p_dec->pf_get_attachments  = DecoderGetInputAttachments;
    p_dec->pf_get_display_date = DecoderGetDisplayDate;
    p_dec->pf_get_display_rate = DecoderGetDisplayRate;
    /* Find a suitable decoder/packetizer module */
    if( !b_packetizer )
        p_dec->p_module = module_need( p_dec, "decoder", "$codec", false );
    else
        p_dec->p_module = module_need( p_dec, "packetizer", "$packetizer", false );
    /* Check if decoder requires already packetized data */
    if( !b_packetizer &&
        p_dec->b_need_packetized && !p_dec->fmt_in.b_packetized )
    {
        p_owner->p_packetizer =
            vlc_custom_create( p_parent, sizeof( decoder_t ), "packetizer" );
        if( p_owner->p_packetizer )
        {
            es_format_Copy( &p_owner->p_packetizer->fmt_in,
                            &p_dec->fmt_in );
            es_format_Copy( &p_owner->p_packetizer->fmt_out,
                            &null_es_format );
            p_owner->p_packetizer->p_module =
                module_need( p_owner->p_packetizer,
                             "packetizer", "$packetizer", false );
            if( !p_owner->p_packetizer->p_module )
            {
                es_format_Clean( &p_owner->p_packetizer->fmt_in );
                vlc_object_release( p_owner->p_packetizer );
                p_owner->p_packetizer = NULL;
            }
        }
    }
    /* Copy ourself the input replay gain */
    if( fmt->i_cat == AUDIO_ES )
    {
        for( unsigned i = 0; i < AUDIO_REPLAY_GAIN_MAX; i++ )
        {
            if( !p_dec->fmt_out.audio_replay_gain.pb_peak[i] )
            {
                p_dec->fmt_out.audio_replay_gain.pb_peak[i] = fmt->audio_replay_gain.pb_peak[i];
                p_dec->fmt_out.audio_replay_gain.pf_peak[i] = fmt->audio_replay_gain.pf_peak[i];
            }
            if( !p_dec->fmt_out.audio_replay_gain.pb_gain[i] )
            {
                p_dec->fmt_out.audio_replay_gain.pb_gain[i] = fmt->audio_replay_gain.pb_gain[i];
                p_dec->fmt_out.audio_replay_gain.pf_gain[i] = fmt->audio_replay_gain.pf_gain[i];
            }
        }
    }
    /* */
    vlc_mutex_init( &p_owner->lock );
    vlc_cond_init( &p_owner->wait_request );
    vlc_cond_init( &p_owner->wait_acknowledge );
    p_owner->b_fmt_description = false;
    es_format_Init( &p_owner->fmt_description, UNKNOWN_ES, 0 );
    p_owner->p_description = NULL;
    p_owner->b_exit = false;
    p_owner->b_paused = false;
    p_owner->pause.i_date = VLC_TS_INVALID;
    p_owner->pause.i_ignore = 0;
    p_owner->b_waiting = false;
    p_owner->b_first = true;
    p_owner->b_has_data = false;
    p_owner->b_flushing = false;
    /* */
    p_owner->cc.b_supported = false;
    if( !b_packetizer )
    {
        if( p_owner->p_packetizer && p_owner->p_packetizer->pf_get_cc )
            p_owner->cc.b_supported = true;
        if( p_dec->pf_get_cc )
            p_owner->cc.b_supported = true;
    }
    for( unsigned i = 0; i < 4; i++ )
    {
        p_owner->cc.pb_present[i] = false;
        p_owner->cc.pp_decoder[i] = NULL;
    }
    p_owner->i_ts_delay = 0;
    return p_dec;
}
