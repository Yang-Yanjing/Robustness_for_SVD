 *****************************************************************************/
static int Create( vlc_object_t *p_this )
{
    int i = 0;
    int i_offset = 0;
    filter_t * p_filter = (filter_t *)p_this;
    filter_sys_t *p_sys;
    /* Validate audio filter format */
    if ( p_filter->fmt_in.audio.i_physical_channels != (AOUT_CHAN_LEFT|AOUT_CHAN_RIGHT)
       || ! ( p_filter->fmt_in.audio.i_original_channels & AOUT_CHAN_DOLBYSTEREO )
       || aout_FormatNbChannels( &p_filter->fmt_out.audio ) <= 2
       || ( p_filter->fmt_in.audio.i_original_channels & ~AOUT_CHAN_DOLBYSTEREO )
          != ( p_filter->fmt_out.audio.i_original_channels & ~AOUT_CHAN_DOLBYSTEREO ) )
    {
        return VLC_EGENERIC;
    }
    if ( p_filter->fmt_in.audio.i_rate != p_filter->fmt_out.audio.i_rate )
    {
        return VLC_EGENERIC;
    }
    if ( p_filter->fmt_in.audio.i_format != VLC_CODEC_FL32
          || p_filter->fmt_out.audio.i_format != VLC_CODEC_FL32 )
    {
        return VLC_EGENERIC;
    }
    /* Allocate the memory needed to store the module's structure */
    p_sys = p_filter->p_sys = malloc( sizeof(*p_sys) );
    if( p_sys == NULL )
        return VLC_ENOMEM;
    p_sys->i_left = -1;
    p_sys->i_center = -1;
    p_sys->i_right = -1;
    p_sys->i_rear_left = -1;
    p_sys->i_rear_center = -1;
    p_sys->i_rear_right = -1;
    while ( pi_vlc_chan_order_wg4[i] )
    {
        if ( p_filter->fmt_out.audio.i_physical_channels & pi_vlc_chan_order_wg4[i] )
        {
            switch ( pi_vlc_chan_order_wg4[i] )
            {
                case AOUT_CHAN_LEFT:
                    p_sys->i_left = i_offset;
                    break;
                case AOUT_CHAN_CENTER:
                    p_sys->i_center = i_offset;
                    break;
                case AOUT_CHAN_RIGHT:
                    p_sys->i_right = i_offset;
                    break;
                case AOUT_CHAN_REARLEFT:
                    p_sys->i_rear_left = i_offset;
                    break;
                case AOUT_CHAN_REARCENTER:
                    p_sys->i_rear_center = i_offset;
                    break;
                case AOUT_CHAN_REARRIGHT:
                    p_sys->i_rear_right = i_offset;
                    break;
            }
            ++i_offset;
        }
        ++i;
    }
    p_filter->pf_audio_filter = DoWork;
    return VLC_SUCCESS;
}
