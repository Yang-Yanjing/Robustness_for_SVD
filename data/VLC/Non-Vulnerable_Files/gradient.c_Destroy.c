 *****************************************************************************/
static void Destroy( vlc_object_t *p_this )
{
    filter_t *p_filter = (filter_t *)p_this;
    filter_sys_t *p_sys = p_filter->p_sys;
    var_DelCallback( p_filter, FILTER_PREFIX "mode",
                     GradientCallback, p_sys );
    var_DelCallback( p_filter, FILTER_PREFIX "type",
                     GradientCallback, p_sys );
    var_DelCallback( p_filter, FILTER_PREFIX "cartoon",
                     GradientCallback, p_sys );
    vlc_mutex_destroy( &p_sys->lock );
    free( p_sys->p_buf32 );
    free( p_sys->p_buf32_bis );
    free( p_sys->p_buf8 );
    free( p_sys->p_pre_hough );
    free( p_sys );
}
