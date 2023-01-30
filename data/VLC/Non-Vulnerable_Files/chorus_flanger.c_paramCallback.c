 ******************************************************************************/
static int paramCallback( vlc_object_t *p_this, char const *psz_var,
                          vlc_value_t oldval, vlc_value_t newval, void *p_data )
{
    filter_t *p_filter = (filter_t *)p_this;
    filter_sys_t *p_sys = (filter_sys_t *) p_data;
    if( !strncmp( psz_var, "delay-time", 10 ) )
    {
        /* if invalid value pretend everything is OK without updating value */
        if( newval.f_float < 0 )
            return VLC_SUCCESS;
        p_sys->f_delayTime = newval.f_float;
        if( !reallocate_buffer( p_filter, p_sys ) )
        {
            p_sys->f_delayTime = oldval.f_float;
            p_sys->i_bufferLength = p_sys->i_channels * ( (int)
                            ( ( p_sys->f_delayTime + p_sys->f_sweepDepth ) * 
                              p_filter->fmt_in.audio.i_rate/1000 ) + 1 );
        }
    }
    else if( !strncmp( psz_var, "sweep-depth", 11 ) )
    {
        if( newval.f_float < 0 || newval.f_float > p_sys->f_delayTime)
            return VLC_SUCCESS;
        p_sys->f_sweepDepth = newval.f_float;
        if( !reallocate_buffer( p_filter, p_sys ) )
        {
            p_sys->f_sweepDepth = oldval.f_float;
            p_sys->i_bufferLength = p_sys->i_channels * ( (int)
                            ( ( p_sys->f_delayTime + p_sys->f_sweepDepth ) * 
                              p_filter->fmt_in.audio.i_rate/1000 ) + 1 );
        }
    }
    else if( !strncmp( psz_var, "sweep-rate", 10 ) )
    {
        if( newval.f_float > p_sys->f_sweepDepth )
            return VLC_SUCCESS;
        p_sys->f_sweepRate = newval.f_float;
        /* Calculate new f_sinMultiplier */
        if( p_sys->f_sweepDepth < small_value() ||
                p_filter->fmt_in.audio.i_rate < small_value() ) {
            p_sys->f_sinMultiplier = 0.0;
        }
        else {
            p_sys->f_sinMultiplier = 11 * p_sys->f_sweepRate /
                ( 7 * p_sys->f_sweepDepth * p_filter->fmt_in.audio.i_rate ) ;
        }
    }
    else if( !strncmp( psz_var, "feedback-gain", 13 ) )
        p_sys->f_feedbackGain = newval.f_float;
    else if( !strncmp( psz_var, "wet-mix", 7 ) )
        p_sys->f_wetLevel = newval.f_float;
    else if( !strncmp( psz_var, "dry-mix", 7 ) )
        p_sys->f_dryLevel = newval.f_float;
    return VLC_SUCCESS;
}
