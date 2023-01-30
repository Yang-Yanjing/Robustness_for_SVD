}
static int aout_update_format( decoder_t *p_dec )
{
    decoder_owner_sys_t *p_owner = p_dec->p_owner;
    if( p_owner->p_aout &&
       ( !AOUT_FMTS_IDENTICAL(&p_dec->fmt_out.audio, &p_owner->audio) ||
         p_dec->fmt_out.i_codec != p_dec->fmt_out.audio.i_format ) )
    {
        audio_output_t *p_aout = p_owner->p_aout;
        /* Parameters changed, restart the aout */
        vlc_mutex_lock( &p_owner->lock );
        aout_DecDelete( p_owner->p_aout );
        p_owner->p_aout = NULL;
        vlc_mutex_unlock( &p_owner->lock );
        input_resource_PutAout( p_owner->p_resource, p_aout );
    }
    if( p_owner->p_aout == NULL )
    {
        const int i_force_dolby = var_InheritInteger( p_dec, "force-dolby-surround" );
        audio_sample_format_t format;
        audio_output_t *p_aout;
        aout_request_vout_t request_vout;
        p_dec->fmt_out.audio.i_format = p_dec->fmt_out.i_codec;
        p_owner->audio = p_dec->fmt_out.audio;
        aout_FormatPrepare( &p_owner->audio );
        memcpy( &format, &p_owner->audio, sizeof( audio_sample_format_t ) );
        if( i_force_dolby &&
            (format.i_original_channels&AOUT_CHAN_PHYSMASK) ==
                (AOUT_CHAN_LEFT|AOUT_CHAN_RIGHT) )
        {
            if( i_force_dolby == 1 )
            {
                format.i_original_channels = format.i_original_channels |
                                             AOUT_CHAN_DOLBYSTEREO;
            }
            else /* i_force_dolby == 2 */
            {
                format.i_original_channels = format.i_original_channels &
                                             ~AOUT_CHAN_DOLBYSTEREO;
            }
        }
        request_vout.pf_request_vout = aout_request_vout;
        request_vout.p_private = p_dec;
        assert( p_owner->p_aout == NULL );
        p_aout = input_resource_GetAout( p_owner->p_resource );
        if( p_aout )
        {
            if( aout_DecNew( p_aout, &format,
                             &p_dec->fmt_out.audio_replay_gain,
                             &request_vout ) )
            {
                input_resource_PutAout( p_owner->p_resource, p_aout );
                p_aout = NULL;
            }
        }
        vlc_mutex_lock( &p_owner->lock );
        p_owner->p_aout = p_aout;
        DecoderUpdateFormatLocked( p_dec );
        if( unlikely(p_owner->b_paused) && p_aout != NULL )
            /* fake pause if needed */
            aout_DecChangePause( p_aout, true, mdate() );
        vlc_mutex_unlock( &p_owner->lock );
        if( p_owner->p_input != NULL )
            input_SendEventAout( p_owner->p_input );
        if( p_aout == NULL )
        {
            msg_Err( p_dec, "failed to create audio output" );
            p_dec->b_error = true;
            return -1;
        }
        p_dec->fmt_out.audio.i_bytes_per_frame =
            p_owner->audio.i_bytes_per_frame;
        p_dec->fmt_out.audio.i_frame_length = p_owner->audio.i_frame_length;
    }
    return 0;
}
