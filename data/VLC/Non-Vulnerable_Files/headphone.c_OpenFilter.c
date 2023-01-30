 *****************************************************************************/
static int OpenFilter( vlc_object_t *p_this )
{
    filter_t *p_filter = (filter_t *)p_this;
    filter_sys_t *p_sys;
    /* Activate this filter only with stereo devices */
    if( p_filter->fmt_out.audio.i_physical_channels
            != (AOUT_CHAN_LEFT|AOUT_CHAN_RIGHT) )
    {
        msg_Dbg( p_filter, "filter discarded (incompatible format)" );
        return VLC_EGENERIC;
    }
    /* Allocate the memory needed to store the module's structure */
    p_sys = p_filter->p_sys = malloc( sizeof(struct filter_sys_t) );
    if( p_sys == NULL )
        return VLC_ENOMEM;
    p_sys->i_overflow_buffer_size = 0;
    p_sys->p_overflow_buffer = NULL;
    p_sys->i_nb_atomic_operations = 0;
    p_sys->p_atomic_operations = NULL;
    if( Init( VLC_OBJECT(p_filter), p_sys
                , aout_FormatNbChannels ( &(p_filter->fmt_in.audio) )
                , p_filter->fmt_in.audio.i_physical_channels
                , p_filter->fmt_in.audio.i_rate ) < 0 )
    {
        free( p_sys );
        return VLC_EGENERIC;
    }
    /* Request a specific format if not already compatible */
    p_filter->fmt_in.audio.i_format = VLC_CODEC_FL32;
    p_filter->fmt_out.audio.i_format = VLC_CODEC_FL32;
    p_filter->fmt_out.audio.i_rate = p_filter->fmt_in.audio.i_rate;
    p_filter->fmt_in.audio.i_original_channels =
                                   p_filter->fmt_out.audio.i_original_channels;
    if( p_filter->fmt_in.audio.i_physical_channels == AOUT_CHANS_STEREO
     && (p_filter->fmt_in.audio.i_original_channels & AOUT_CHAN_DOLBYSTEREO)
     && !var_InheritBool( p_filter, "headphone-dolby" ) )
    {
        p_filter->fmt_in.audio.i_physical_channels = AOUT_CHANS_5_0;
    }
    p_filter->pf_audio_filter = Convert;
    return VLC_SUCCESS;
}
