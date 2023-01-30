} entrant = { false, VLC_STATIC_MUTEX, };
static int OpenEncoder( vlc_object_t *p_this )
{
    encoder_t *p_enc = (encoder_t*)p_this;
    encoder_sys_t *p_sys;
    /* shine is an 'MP3' encoder */
    if( (p_enc->fmt_out.i_codec != VLC_CODEC_MP3 && p_enc->fmt_out.i_codec != VLC_CODEC_MPGA) ||
        p_enc->fmt_out.audio.i_channels > 2 )
        return VLC_EGENERIC;
    /* Shine is strict on its input */
    if( p_enc->fmt_in.audio.i_channels != 2 )
    {
        msg_Err( p_enc, "Only stereo input is accepted, rejecting %d channels",
            p_enc->fmt_in.audio.i_channels );
        return VLC_EGENERIC;
    }
    if( p_enc->fmt_out.i_bitrate <= 0 )
    {
        msg_Err( p_enc, "unknown bitrate" );
        return VLC_EGENERIC;
    }
    msg_Dbg( p_enc, "bitrate %d, samplerate %d, channels %d",
             p_enc->fmt_out.i_bitrate, p_enc->fmt_out.audio.i_rate,
             p_enc->fmt_out.audio.i_channels );
    vlc_mutex_lock( &entrant.lock );
    if( entrant.busy )
    {
        msg_Err( p_enc, "encoder already in progress" );
        vlc_mutex_unlock( &entrant.lock );
        return VLC_EGENERIC;
    }
    entrant.busy = true;
    vlc_mutex_unlock( &entrant.lock );
    p_enc->p_sys = p_sys = calloc( 1, sizeof( *p_sys ) );
    if( !p_sys )
        goto enomem;
    if( !( p_sys->p_fifo = block_FifoNew() ) )
    {
        free( p_sys );
        goto enomem;
    }
    shine_config_t cfg = {
        .wave = {
            .channels = p_enc->fmt_out.audio.i_channels,
            .samplerate = p_enc->fmt_out.audio.i_rate,
        },
    };
    shine_set_config_mpeg_defaults(&cfg.mpeg);
    cfg.mpeg.bitr = p_enc->fmt_out.i_bitrate / 1000;
    if (shine_check_config(cfg.wave.samplerate, cfg.mpeg.bitr) == -1) {
        msg_Err(p_enc, "Invalid bitrate %d\n", cfg.mpeg.bitr);
        free(p_sys);
        return VLC_EGENERIC;
    }
    p_sys->s = shine_initialise(&cfg);
    p_sys->samples_per_frame = shine_samples_per_pass(p_sys->s);
    p_enc->pf_encode_audio = EncodeFrame;
    p_enc->fmt_out.i_cat = AUDIO_ES;
    p_enc->fmt_in.i_codec = VLC_CODEC_S16N;
    return VLC_SUCCESS;
enomem:
    vlc_mutex_lock( &entrant.lock );
    entrant.busy = false;
    vlc_mutex_unlock( &entrant.lock );
    return VLC_ENOMEM;
}
