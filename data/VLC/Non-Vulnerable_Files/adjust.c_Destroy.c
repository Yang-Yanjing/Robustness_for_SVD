 *****************************************************************************/
static void Destroy( vlc_object_t *p_this )
{
    filter_t *p_filter = (filter_t *)p_this;
    filter_sys_t *p_sys = p_filter->p_sys;
    var_DelCallback( p_filter, "contrast",   AdjustCallback, p_sys );
    var_DelCallback( p_filter, "brightness", AdjustCallback, p_sys );
    var_DelCallback( p_filter, "hue",        AdjustCallback, p_sys );
    var_DelCallback( p_filter, "saturation", AdjustCallback, p_sys );
    var_DelCallback( p_filter, "gamma",      AdjustCallback, p_sys );
    var_DelCallback( p_filter, "brightness-threshold",
                                             AdjustCallback, p_sys );
    vlc_mutex_destroy( &p_sys->lock );
    free( p_sys );
}
