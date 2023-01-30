 */
static void Close( vlc_object_t *p_this )
{
    filter_t *p_filter = ( filter_t* )p_this;
    filter_sys_t *p_sys = p_filter->p_sys;
    var_DelCallback( p_this, "delay-time", paramCallback, p_sys );
    var_DelCallback( p_this, "sweep-depth", paramCallback, p_sys );
    var_DelCallback( p_this, "sweep-rate", paramCallback, p_sys );
    var_DelCallback( p_this, "feedback-gain", paramCallback, p_sys );
    var_DelCallback( p_this, "wet-mix", paramCallback, p_sys );
    var_DelCallback( p_this, "dry-mix", paramCallback, p_sys );
    var_Destroy( p_this, "delay-time" );
    var_Destroy( p_this, "sweep-depth" );
    var_Destroy( p_this, "sweep-rate" );
    var_Destroy( p_this, "feedback-gain" );
    var_Destroy( p_this, "wet-mix" );
    var_Destroy( p_this, "dry-mix" );
    free( p_sys->p_delayLineStart );
    free( p_sys );
}
