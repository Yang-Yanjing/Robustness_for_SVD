 *****************************************************************************/
static int Open( vlc_object_t *p_this )
{
    decoder_t *p_dec = (decoder_t*)p_this;
    decoder_sys_t *p_sys;
    faacDecConfiguration *cfg;
    if( p_dec->fmt_in.i_codec != VLC_CODEC_MP4A )
    {
        return VLC_EGENERIC;
    }
    /* Allocate the memory needed to store the decoder's structure */
    if( ( p_dec->p_sys = p_sys = malloc( sizeof(*p_sys) ) ) == NULL )
        return VLC_ENOMEM;
    /* Open a faad context */
    if( ( p_sys->hfaad = faacDecOpen() ) == NULL )
    {
        msg_Err( p_dec, "cannot initialize faad" );
        free( p_sys );
        return VLC_EGENERIC;
    }
    /* Misc init */
    date_Set( &p_sys->date, 0 );
    p_dec->fmt_out.i_cat = AUDIO_ES;
    p_dec->fmt_out.i_codec = HAVE_FPU ? VLC_CODEC_FL32 : VLC_CODEC_S16N;
    p_dec->fmt_out.audio.i_physical_channels =
        p_dec->fmt_out.audio.i_original_channels = 0;
    if( p_dec->fmt_in.i_extra > 0 )
    {
        /* We have a decoder config so init the handle */
        unsigned long i_rate;
        unsigned char i_channels;
        if( faacDecInit2( p_sys->hfaad, p_dec->fmt_in.p_extra,
                          p_dec->fmt_in.i_extra,
                          &i_rate, &i_channels ) < 0 )
        {
            msg_Err( p_dec, "Failed to initialize faad using extra data" );
            faacDecClose( p_sys->hfaad );
            free( p_sys );
            return VLC_EGENERIC;
        }
        p_dec->fmt_out.audio.i_rate = i_rate;
        p_dec->fmt_out.audio.i_channels = i_channels;
        p_dec->fmt_out.audio.i_physical_channels
            = p_dec->fmt_out.audio.i_original_channels
            = pi_channels_guessed[i_channels];
        date_Init( &p_sys->date, i_rate, 1 );
    }
    else
    {
        /* Will be initalised from first frame */
        p_dec->fmt_out.audio.i_rate = 0;
        p_dec->fmt_out.audio.i_channels = 0;
    }
    /* Set the faad config */
    cfg = faacDecGetCurrentConfiguration( p_sys->hfaad );
    if( p_dec->fmt_in.audio.i_rate )
        cfg->defSampleRate = p_dec->fmt_in.audio.i_rate;
    cfg->outputFormat = HAVE_FPU ? FAAD_FMT_FLOAT : FAAD_FMT_16BIT;
    faacDecSetConfiguration( p_sys->hfaad, cfg );
    /* buffer */
    p_sys->i_buffer = p_sys->i_buffer_size = 0;
    p_sys->p_buffer = NULL;
    /* Faad2 can't deal with truncated data (eg. from MPEG TS) */
    p_dec->b_need_packetized = true;
    p_sys->b_sbr = p_sys->b_ps = false;
    p_dec->pf_decode_audio = DecodeBlock;
    return VLC_SUCCESS;
}
