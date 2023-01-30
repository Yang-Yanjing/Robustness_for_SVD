 */
static int Open( vlc_object_t *p_this )
{
    filter_t *p_filter = (filter_t*)p_this;
    filter_sys_t *p_sys = p_filter->p_sys = malloc( sizeof( *p_sys ) );
    if( !p_sys )
        return VLC_ENOMEM;
    p_sys->i_channels       = aout_FormatNbChannels( &p_filter->fmt_in.audio );
    p_sys->f_delayTime      = var_CreateGetFloat( p_this, "delay-time" );
    p_sys->f_sweepDepth     = var_CreateGetFloat( p_this, "sweep-depth" );
    p_sys->f_sweepRate      = var_CreateGetFloat( p_this, "sweep-rate" );
    p_sys->f_feedbackGain   = var_CreateGetFloat( p_this, "feedback-gain" );
    p_sys->f_dryLevel       = var_CreateGetFloat( p_this, "dry-mix" );
    p_sys->f_wetLevel       = var_CreateGetFloat( p_this, "wet-mix" );
    var_AddCallback( p_this, "delay-time", paramCallback, p_sys );
    var_AddCallback( p_this, "sweep-depth", paramCallback, p_sys );
    var_AddCallback( p_this, "sweep-rate", paramCallback, p_sys );
    var_AddCallback( p_this, "feedback-gain", paramCallback, p_sys );
    var_AddCallback( p_this, "dry-mix", paramCallback, p_sys );
    var_AddCallback( p_this, "wet-mix", paramCallback, p_sys );
    if( p_sys->f_delayTime < 0.0)
    {
        msg_Err( p_filter, "Delay Time is invalid" );
        free(p_sys);
        return VLC_EGENERIC;
    }
    if( p_sys->f_sweepDepth > p_sys->f_delayTime || p_sys->f_sweepDepth < 0.0 )
    {
        msg_Err( p_filter, "Sweep Depth is invalid" );
        free( p_sys );
        return VLC_EGENERIC;
    }
    if( p_sys->f_sweepRate < 0.0 )
    {
        msg_Err( p_filter, "Sweep Rate is invalid" );
        free( p_sys );
        return VLC_EGENERIC;
    }
    /* Max delay = delay + depth. Min = delay - depth */
    p_sys->i_bufferLength = p_sys->i_channels * ( (int)( ( p_sys->f_delayTime
                + p_sys->f_sweepDepth ) * p_filter->fmt_in.audio.i_rate/1000 ) + 1 );
    msg_Dbg( p_filter , "Buffer length:%d, Channels:%d, Sweep Depth:%f, Delay "
            "time:%f, Sweep Rate:%f, Sample Rate: %d", p_sys->i_bufferLength,
            p_sys->i_channels, p_sys->f_sweepDepth, p_sys->f_delayTime,
            p_sys->f_sweepRate, p_filter->fmt_in.audio.i_rate );
    if( p_sys->i_bufferLength <= 0 )
    {
        msg_Err( p_filter, "Delay-time, Sample rate or Channels was incorrect" );
        free(p_sys);
        return VLC_EGENERIC;
    }
    p_sys->p_delayLineStart = calloc( p_sys->i_bufferLength, sizeof( float ) );
    if( !p_sys->p_delayLineStart )
    {
        free( p_sys );
        return VLC_ENOMEM;
    }
    p_sys->i_cumulative = 0;
    p_sys->i_step = p_sys->f_sweepRate > 0 ? 1 : 0;
    p_sys->f_offset = 0;
    p_sys->f_temp = 0;
    p_sys->p_delayLineEnd = p_sys->p_delayLineStart + p_sys->i_bufferLength;
    p_sys->p_write = p_sys->p_delayLineStart;
    if( p_sys->f_sweepDepth < small_value() ||
            p_filter->fmt_in.audio.i_rate < small_value() ) {
        p_sys->f_sinMultiplier = 0.0;
    }
    else {
        p_sys->f_sinMultiplier = 11 * p_sys->f_sweepRate /
            ( 7 * p_sys->f_sweepDepth * p_filter->fmt_in.audio.i_rate ) ;
    }
    p_sys->i_sampleRate = p_filter->fmt_in.audio.i_rate;
    p_filter->fmt_in.audio.i_format = VLC_CODEC_FL32;
    p_filter->fmt_out.audio = p_filter->fmt_in.audio;
    p_filter->pf_audio_filter = DoWork;
    return VLC_SUCCESS;
}
