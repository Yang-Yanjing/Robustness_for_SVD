 *****************************************************************************/
static void Destroy( vlc_object_t *p_this )
{
    filter_t *p_filter = (filter_t *)p_this;
    filter_sys_t *p_sys = p_filter->p_sys;
    var_DelCallback( p_filter, FILTER_PREFIX "color",
                     ballCallback, p_sys );
    var_DelCallback( p_filter, FILTER_PREFIX "size",
                     ballCallback, p_sys );
    var_DelCallback( p_filter, FILTER_PREFIX "speed",
                     ballCallback, p_sys );
    var_DelCallback( p_filter, FILTER_PREFIX "edge-visible",
                     ballCallback, p_sys );
    vlc_mutex_destroy( &p_sys->lock );
    image_HandlerDelete( p_sys->p_image );
    free( p_sys->p_smooth );
    free( p_sys->p_grad_x );
    free( p_sys->p_grad_y );
    free( p_sys );
}
