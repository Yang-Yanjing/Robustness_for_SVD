 *****************************************************************************/
static int Open( vlc_object_t *p_this )
{
    filter_t *p_filter = (filter_t*)p_this;
    vlc_object_t *p_aout = p_filter->p_parent;
    float f_sample_rate = p_filter->fmt_in.audio.i_rate;
    float f_num;
    /* Initialize the filter parameter structure */
    filter_sys_t *p_sys = p_filter->p_sys = calloc( 1, sizeof(*p_sys) );
    if( !p_sys )
    {
        return VLC_ENOMEM;
    }
    /* Initialize the attack lookup table */
    p_sys->pf_as[0] = 1.0f;
    for( int i = 1; i < A_TBL; i++ )
    {
        p_sys->pf_as[i] = expf( -1.0f / ( f_sample_rate * i / A_TBL ) );
    }
    /* Calculate the RMS and lookahead sizes from the sample rate */
    f_num = 0.01f * f_sample_rate;
    p_sys->rms.i_count = Round( Clamp( 0.5f * f_num, 1.0f, RMS_BUF_SIZE ) );
    p_sys->la.i_count = Round( Clamp( f_num, 1.0f, LOOKAHEAD_SIZE ) );
    /* Initialize decibel lookup tables */
    DbInit( p_sys );
    /* Restore the last saved settings */
    p_sys->f_rms_peak    = var_CreateGetFloat( p_aout, "compressor-rms-peak" );
    p_sys->f_attack      = var_CreateGetFloat( p_aout, "compressor-attack" );
    p_sys->f_release     = var_CreateGetFloat( p_aout, "compressor-release" );
    p_sys->f_threshold   = var_CreateGetFloat( p_aout, "compressor-threshold" );
    p_sys->f_ratio       = var_CreateGetFloat( p_aout, "compressor-ratio" );
    p_sys->f_knee        = var_CreateGetFloat( p_aout, "compressor-knee" );
    p_sys->f_makeup_gain =
           var_CreateGetFloat( p_aout, "compressor-makeup-gain" );
    /* Initialize the mutex */
    vlc_mutex_init( &p_sys->lock );
    /* Add our own callbacks */
    var_AddCallback( p_aout, "compressor-rms-peak", RMSPeakCallback, p_sys );
    var_AddCallback( p_aout, "compressor-attack", AttackCallback, p_sys );
    var_AddCallback( p_aout, "compressor-release", ReleaseCallback, p_sys );
    var_AddCallback( p_aout, "compressor-threshold", ThresholdCallback, p_sys );
    var_AddCallback( p_aout, "compressor-ratio", RatioCallback, p_sys );
    var_AddCallback( p_aout, "compressor-knee", KneeCallback, p_sys );
    var_AddCallback( p_aout, "compressor-makeup-gain", MakeupGainCallback, p_sys );
    /* Set the filter function */
    p_filter->fmt_in.audio.i_format = VLC_CODEC_FL32;
    p_filter->fmt_out.audio = p_filter->fmt_in.audio;
    p_filter->pf_audio_filter = DoWork;
    /* At this stage, we are ready! */
    msg_Dbg( p_filter, "compressor successfully initialized" );
    return VLC_SUCCESS;
}
