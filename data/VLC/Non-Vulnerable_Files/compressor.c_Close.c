 *****************************************************************************/
static void Close( vlc_object_t *p_this )
{
    filter_t *p_filter = (filter_t*)p_this;
    vlc_object_t *p_aout = p_filter->p_parent;
    filter_sys_t *p_sys = p_filter->p_sys;
    /* Remove our callbacks */
    var_DelCallback( p_aout, "compressor-rms-peak", RMSPeakCallback, p_sys );
    var_DelCallback( p_aout, "compressor-attack", AttackCallback, p_sys );
    var_DelCallback( p_aout, "compressor-release", ReleaseCallback, p_sys );
    var_DelCallback( p_aout, "compressor-threshold", ThresholdCallback, p_sys );
    var_DelCallback( p_aout, "compressor-ratio", RatioCallback, p_sys );
    var_DelCallback( p_aout, "compressor-knee", KneeCallback, p_sys );
    var_DelCallback( p_aout, "compressor-makeup-gain", MakeupGainCallback, p_sys );
    /* Destroy the mutex */
    vlc_mutex_destroy( &p_sys->lock );
    /* Destroy the filter parameter structure */
    free( p_sys );
}
