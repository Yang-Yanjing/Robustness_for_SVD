 *****************************************************************************/
static int Open( vlc_object_t *p_this, filter_sys_t *p_sys,
                 const audio_format_t *restrict input,
                 const audio_format_t *restrict output )
{
    p_sys->b_dynrng = var_InheritBool( p_this, "a52-dynrng" );
    p_sys->b_dontwarn = 0;
    /* No upmixing: it's not necessary and some other filters may want to do
     * it themselves. */
    if ( aout_FormatNbChannels( output ) > aout_FormatNbChannels( input ) )
    {
        if ( ! var_InheritBool( p_this, "a52-upmix" ) )
        {
            return VLC_EGENERIC;
        }
    }
    /* We'll do our own downmixing, thanks. */
    p_sys->i_nb_channels = aout_FormatNbChannels( output );
    switch ( output->i_physical_channels & ~AOUT_CHAN_LFE )
    {
    case AOUT_CHAN_CENTER:
        if ( (output->i_original_channels & AOUT_CHAN_CENTER)
              || (output->i_original_channels
                   & (AOUT_CHAN_LEFT | AOUT_CHAN_RIGHT)) )
        {
            p_sys->i_flags = A52_MONO;
        }
        else if ( output->i_original_channels & AOUT_CHAN_LEFT )
        {
            p_sys->i_flags = A52_CHANNEL1;
        }
        else
        {
            p_sys->i_flags = A52_CHANNEL2;
        }
        break;
    case AOUT_CHAN_LEFT | AOUT_CHAN_RIGHT:
        if ( output->i_original_channels & AOUT_CHAN_DOLBYSTEREO )
        {
            p_sys->i_flags = A52_DOLBY;
        }
        else if ( input->i_original_channels == AOUT_CHAN_CENTER )
        {
            p_sys->i_flags = A52_MONO;
        }
        else if ( input->i_original_channels & AOUT_CHAN_DUALMONO )
        {
            p_sys->i_flags = A52_CHANNEL;
        }
        else if ( !(output->i_original_channels & AOUT_CHAN_RIGHT) )
        {
            p_sys->i_flags = A52_CHANNEL1;
        }
        else if ( !(output->i_original_channels & AOUT_CHAN_LEFT) )
        {
            p_sys->i_flags = A52_CHANNEL2;
        }
        else
        {
            p_sys->i_flags = A52_STEREO;
        }
        break;
    case AOUT_CHAN_LEFT | AOUT_CHAN_RIGHT | AOUT_CHAN_CENTER:
        p_sys->i_flags = A52_3F;
        break;
    case AOUT_CHAN_LEFT | AOUT_CHAN_RIGHT | AOUT_CHAN_REARCENTER:
        p_sys->i_flags = A52_2F1R;
        break;
    case AOUT_CHAN_LEFT | AOUT_CHAN_RIGHT | AOUT_CHAN_CENTER
          | AOUT_CHAN_REARCENTER:
        p_sys->i_flags = A52_3F1R;
        break;
    case AOUT_CHAN_LEFT | AOUT_CHAN_RIGHT
          | AOUT_CHAN_REARLEFT | AOUT_CHAN_REARRIGHT:
        p_sys->i_flags = A52_2F2R;
        break;
    case AOUT_CHAN_LEFT | AOUT_CHAN_RIGHT | AOUT_CHAN_CENTER
          | AOUT_CHAN_REARLEFT | AOUT_CHAN_REARRIGHT:
        p_sys->i_flags = A52_3F2R;
        break;
    default:
        msg_Warn( p_this, "unknown sample format!" );
        free( p_sys );
        return VLC_EGENERIC;
    }
    if ( output->i_physical_channels & AOUT_CHAN_LFE )
    {
        p_sys->i_flags |= A52_LFE;
    }
    p_sys->i_flags |= A52_ADJUST_LEVEL;
    /* Initialize liba52 */
    p_sys->p_liba52 = a52_init( 0 );
    if( p_sys->p_liba52 == NULL )
    {
        msg_Err( p_this, "unable to initialize liba52" );
        free( p_sys );
        return VLC_EGENERIC;
    }
    aout_CheckChannelReorder( pi_channels_in, NULL,
                              output->i_physical_channels,
                              p_sys->pi_chan_table );
    return VLC_SUCCESS;
}
