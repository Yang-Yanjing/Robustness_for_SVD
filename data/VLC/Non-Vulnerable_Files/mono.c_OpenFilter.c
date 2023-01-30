 *****************************************************************************/
static int OpenFilter( vlc_object_t *p_this )
{
    filter_t * p_filter = (filter_t *)p_this;
    filter_sys_t *p_sys = NULL;
    if( aout_FormatNbChannels( &(p_filter->fmt_in.audio) ) == 1 )
    {
        /*msg_Dbg( p_filter, "filter discarded (incompatible format)" );*/
        return VLC_EGENERIC;
    }
    p_filter->fmt_in.audio.i_format = VLC_CODEC_S16N;
    p_filter->fmt_out.audio.i_format = VLC_CODEC_S16N;
    /* Allocate the memory needed to store the module's structure */
    p_sys = p_filter->p_sys = malloc( sizeof(filter_sys_t) );
    if( p_sys == NULL )
        return VLC_EGENERIC;
    p_sys->b_downmix = var_InheritBool( p_this, MONO_CFG "downmix" );
    p_sys->i_channel_selected = var_InheritInteger( p_this, MONO_CFG "channel" );
    p_sys->i_nb_channels = aout_FormatNbChannels( &(p_filter->fmt_in.audio) );
    p_sys->i_bitspersample = p_filter->fmt_out.audio.i_bitspersample;
    p_sys->i_overflow_buffer_size = 0;
    p_sys->p_overflow_buffer = NULL;
    p_sys->i_nb_atomic_operations = 0;
    p_sys->p_atomic_operations = NULL;
    if( Init( VLC_OBJECT(p_filter), p_filter->p_sys,
              aout_FormatNbChannels( &p_filter->fmt_in.audio ),
              p_filter->fmt_in.audio.i_physical_channels,
              p_filter->fmt_in.audio.i_rate ) < 0 )
    {
        free( p_sys );
        return VLC_EGENERIC;
    }
    if( p_sys->b_downmix )
    {
        msg_Dbg( p_this, "using stereo to mono downmix" );
        p_filter->fmt_out.audio.i_physical_channels = AOUT_CHAN_CENTER;
        p_filter->fmt_out.audio.i_channels = 1;
    }
    else
    {
        msg_Dbg( p_this, "using pseudo mono" );
        p_filter->fmt_out.audio.i_physical_channels = AOUT_CHANS_STEREO;
        p_filter->fmt_out.audio.i_channels = 2;
    }
    p_filter->fmt_out.audio.i_rate = p_filter->fmt_in.audio.i_rate;
    p_filter->pf_audio_filter = Convert;
    msg_Dbg( p_this, "%4.4s->%4.4s, channels %d->%d, bits per sample: %i->%i",
             (char *)&p_filter->fmt_in.i_codec,
             (char *)&p_filter->fmt_out.i_codec,
             p_filter->fmt_in.audio.i_physical_channels,
             p_filter->fmt_out.audio.i_physical_channels,
             p_filter->fmt_in.audio.i_bitspersample,
             p_filter->fmt_out.audio.i_bitspersample );
    return VLC_SUCCESS;
}
