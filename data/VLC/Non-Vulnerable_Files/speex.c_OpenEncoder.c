 *****************************************************************************/
static int OpenEncoder( vlc_object_t *p_this )
{
    encoder_t *p_enc = (encoder_t *)p_this;
    encoder_sys_t *p_sys;
    const SpeexMode *p_speex_mode = &speex_nb_mode;
    int i_tmp, i;
    const char *pp_header[2];
    int pi_header[2];
    uint8_t *p_extra;
    if( p_enc->fmt_out.i_codec != VLC_CODEC_SPEEX &&
        !p_enc->b_force )
    {
        return VLC_EGENERIC;
    }
    config_ChainParse( p_enc, ENC_CFG_PREFIX, ppsz_enc_options, p_enc->p_cfg );
    switch( var_GetInteger( p_enc, ENC_CFG_PREFIX "mode" ) )
    {
    case 1:
        msg_Dbg( p_enc, "Using wideband" );
        p_speex_mode = &speex_wb_mode;
        break;
    case 2:
        msg_Dbg( p_enc, "Using ultra-wideband" );
        p_speex_mode = &speex_uwb_mode;
        break;
    default:
        msg_Dbg( p_enc, "Using narrowband" );
        p_speex_mode = &speex_nb_mode;
        break;
    }
    /* Allocate the memory needed to store the decoder's structure */
    if( ( p_sys = (encoder_sys_t *)malloc(sizeof(encoder_sys_t)) ) == NULL )
        return VLC_ENOMEM;
    p_enc->p_sys = p_sys;
    p_enc->pf_encode_audio = Encode;
    p_enc->fmt_in.i_codec = VLC_CODEC_S16N;
    p_enc->fmt_out.i_codec = VLC_CODEC_SPEEX;
    speex_init_header( &p_sys->header, p_enc->fmt_in.audio.i_rate,
                       1, p_speex_mode );
    p_sys->header.frames_per_packet = 1;
    p_sys->header.vbr = var_GetBool( p_enc, ENC_CFG_PREFIX "cbr" ) ? 0 : 1;
    p_sys->header.nb_channels = p_enc->fmt_in.audio.i_channels;
    /* Create a new encoder state in narrowband mode */
    p_sys->p_state = speex_encoder_init( p_speex_mode );
    /* Parameters */
    i_tmp = var_GetInteger( p_enc, ENC_CFG_PREFIX "complexity" );
    speex_encoder_ctl( p_sys->p_state, SPEEX_SET_COMPLEXITY, &i_tmp );
    i_tmp = var_GetBool( p_enc, ENC_CFG_PREFIX "cbr" ) ? 0 : 1;
    speex_encoder_ctl( p_sys->p_state, SPEEX_SET_VBR, &i_tmp );
    if( i_tmp == 0 ) /* CBR */
    {
        i_tmp = var_GetFloat( p_enc, ENC_CFG_PREFIX "quality" );
        speex_encoder_ctl( p_sys->p_state, SPEEX_SET_QUALITY, &i_tmp );
        i_tmp = var_GetBool( p_enc, ENC_CFG_PREFIX "vad" ) ? 1 : 0;
        speex_encoder_ctl( p_sys->p_state, SPEEX_SET_VAD, &i_tmp );
    }
    else
    {
        float f_tmp;
        f_tmp = var_GetFloat( p_enc, ENC_CFG_PREFIX "quality" );
        speex_encoder_ctl( p_sys->p_state, SPEEX_SET_VBR_QUALITY, &f_tmp );
        i_tmp = var_GetInteger( p_enc, ENC_CFG_PREFIX "max-bitrate" );
        if( i_tmp > 0 )
#ifdef SPEEX_SET_VBR_MAX_BITRATE
            speex_encoder_ctl( p_sys->p_state, SPEEX_SET_VBR_MAX_BITRATE, &i_tmp );
#else
            msg_Dbg( p_enc, "max-bitrate cannot be set in this version of libspeex");
#endif
    }
    i_tmp = var_GetBool( p_enc, ENC_CFG_PREFIX "dtx" ) ? 1 : 0;
    speex_encoder_ctl( p_sys->p_state, SPEEX_SET_DTX, &i_tmp );
    /*Initialization of the structure that holds the bits*/
    speex_bits_init( &p_sys->bits );
    p_sys->i_frames_in_packet = 0;
    p_sys->i_samples_delay = 0;
    speex_encoder_ctl( p_sys->p_state, SPEEX_GET_FRAME_SIZE,
                       &p_sys->i_frame_length );
    p_sys->i_frame_size = p_sys->i_frame_length *
        sizeof(int16_t) * p_enc->fmt_in.audio.i_channels;
    p_sys->p_buffer = xmalloc( p_sys->i_frame_size );
    /* Create and store headers */
    pp_header[0] = speex_header_to_packet( &p_sys->header, &pi_header[0] );
    pp_header[1] = "ENCODER=VLC media player";
    pi_header[1] = sizeof("ENCODER=VLC media player");
    p_enc->fmt_out.i_extra = 3 * 2 + pi_header[0] + pi_header[1];
    p_extra = p_enc->fmt_out.p_extra = xmalloc( p_enc->fmt_out.i_extra );
    for( i = 0; i < 2; i++ )
    {
        *(p_extra++) = pi_header[i] >> 8;
        *(p_extra++) = pi_header[i] & 0xFF;
        memcpy( p_extra, pp_header[i], pi_header[i] );
        p_extra += pi_header[i];
    }
    msg_Dbg( p_enc, "encoding: frame size:%d, channels:%d, samplerate:%d",
             p_sys->i_frame_size, p_enc->fmt_in.audio.i_channels,
             p_enc->fmt_in.audio.i_rate );
    return VLC_SUCCESS;
}
