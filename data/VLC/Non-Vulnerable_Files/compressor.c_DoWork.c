 *****************************************************************************/
static block_t * DoWork( filter_t * p_filter, block_t * p_in_buf )
{
    int i_samples = p_in_buf->i_nb_samples;
    int i_channels = aout_FormatNbChannels( &p_filter->fmt_in.audio );
    float *pf_buf = (float*)p_in_buf->p_buffer;
    /* Current parameters */
    filter_sys_t *p_sys = p_filter->p_sys;
    /* Fetch the configurable parameters */
    vlc_mutex_lock( &p_sys->lock );
    float f_rms_peak    = p_sys->f_rms_peak;     /* RMS/peak */
    float f_attack      = p_sys->f_attack;       /* Attack time (ms) */
    float f_release     = p_sys->f_release;      /* Release time (ms) */
    float f_threshold   = p_sys->f_threshold;    /* Threshold level (dB) */
    float f_ratio       = p_sys->f_ratio;        /* Ratio (n:1) */
    float f_knee        = p_sys->f_knee;         /* Knee radius (dB) */
    float f_makeup_gain = p_sys->f_makeup_gain;  /* Makeup gain (dB) */
    vlc_mutex_unlock( &p_sys->lock );
    /* Fetch the internal parameters */
    float f_amp      =  p_sys->f_amp;
    float *pf_as     =  p_sys->pf_as;
    float f_env      =  p_sys->f_env;
    float f_env_peak =  p_sys->f_env_peak;
    float f_env_rms  =  p_sys->f_env_rms;
    float f_gain     =  p_sys->f_gain;
    float f_gain_out =  p_sys->f_gain_out;
    rms_env *p_rms   = &p_sys->rms;
    float f_sum      =  p_sys->f_sum;
    lookahead *p_la  = &p_sys->la;
    /* Prepare other compressor parameters */
    float f_ga       = f_attack < 2.0f ? 0.0f :
                       pf_as[Round( f_attack  * 0.001f * ( A_TBL - 1 ) )];
    float f_gr       = pf_as[Round( f_release * 0.001f * ( A_TBL - 1 ) )];
    float f_rs       = ( f_ratio - 1.0f ) / f_ratio;
    float f_mug      = Db2Lin( f_makeup_gain, p_sys );
    float f_knee_min = Db2Lin( f_threshold - f_knee, p_sys );
    float f_knee_max = Db2Lin( f_threshold + f_knee, p_sys );
    float f_ef_a     = f_ga * 0.25f;
    float f_ef_ai    = 1.0f - f_ef_a;
    /* Process the current buffer */
    for( int i = 0; i < i_samples; i++ )
    {
        float f_lev_in_old, f_lev_in_new;
        /* Now, compress the pre-equalized audio (ported from sc4_1882
         * plugin with a few modifications) */
        /* Fetch the old delayed buffer value */
        f_lev_in_old = p_la->p_buf[p_la->i_pos].f_lev_in;
        /* Find the peak value of current sample.  This becomes the new delayed
         * buffer value that replaces the old one in the lookahead array */
        f_lev_in_new = fabs( pf_buf[0] );
        for( int i_chan = 1; i_chan < i_channels; i_chan++ )
        {
            f_lev_in_new = Max( f_lev_in_new, fabs( pf_buf[i_chan] ) );
        }
        p_la->p_buf[p_la->i_pos].f_lev_in = f_lev_in_new;
        /* Add the square of the peak value to a running sum */
        f_sum += f_lev_in_new * f_lev_in_new;
        /* Update the RMS envelope */
        if( f_amp > f_env_rms )
        {
            f_env_rms = f_env_rms * f_ga + f_amp * ( 1.0f - f_ga );
        }
        else
        {
            f_env_rms = f_env_rms * f_gr + f_amp * ( 1.0f - f_gr );
        }
        RoundToZero( &f_env_rms );
        /* Update the peak envelope */
        if( f_lev_in_old > f_env_peak )
        {
            f_env_peak = f_env_peak * f_ga + f_lev_in_old * ( 1.0f - f_ga );
        }
        else
        {
            f_env_peak = f_env_peak * f_gr + f_lev_in_old * ( 1.0f - f_gr );
        }
        RoundToZero( &f_env_peak );
        /* Process the RMS value and update the output gain every 4 samples */
        if( ( p_sys->i_count++ & 3 ) == 3 )
        {
            /* Process the RMS value by placing in the mean square value, and
             * reset the running sum */
            f_amp = RmsEnvProcess( p_rms, f_sum * 0.25f );
            f_sum = 0.0f;
            if( isnan( f_env_rms ) )
            {
                /* This can happen sometimes, but I don't know why. */
                f_env_rms = 0.0f;
            }
            /* Find the superposition of the RMS and peak envelopes */
            f_env = LIN_INTERP( f_rms_peak, f_env_rms, f_env_peak );
            /* Update the output gain */
            if( f_env <= f_knee_min )
            {
                /* Gain below the knee (and below the threshold) */
                f_gain_out = 1.0f;
            }
            else if( f_env < f_knee_max )
            {
                /* Gain within the knee */
                const float f_x = -( f_threshold
                                   - f_knee - Lin2Db( f_env, p_sys ) ) / f_knee;
                f_gain_out = Db2Lin( -f_knee * f_rs * f_x * f_x * 0.25f,
                                      p_sys );
            }
            else
            {
                /* Gain above the knee (and above the threshold) */
                f_gain_out = Db2Lin( ( f_threshold - Lin2Db( f_env, p_sys ) )
                                     * f_rs, p_sys );
            }
        }
        /* Find the total gain */
        f_gain = f_gain * f_ef_a + f_gain_out * f_ef_ai;
        /* Write the resulting buffer to the output */
        BufferProcess( pf_buf, i_channels, f_gain, f_mug, p_la );
        pf_buf += i_channels;
    }
    /* Update the internal parameters */
    p_sys->f_sum      = f_sum;
    p_sys->f_amp      = f_amp;
    p_sys->f_gain     = f_gain;
    p_sys->f_gain_out = f_gain_out;
    p_sys->f_env      = f_env;
    p_sys->f_env_rms  = f_env_rms;
    p_sys->f_env_peak = f_env_peak;
    return p_in_buf;
}
