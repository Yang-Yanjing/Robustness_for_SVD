 *****************************************************************************/
static void Destroy( vlc_object_t *p_this )
{
    filter_t *p_filter = (filter_t *)p_this;
    filter_sys_t *p_sys = p_filter->p_sys;
    if( p_sys->p_mask )
        picture_Release( p_sys->p_mask );
    var_DelCallback( p_filter, CFG_PREFIX "x", EraseCallback, p_sys );
    var_DelCallback( p_filter, CFG_PREFIX "y", EraseCallback, p_sys );
    var_DelCallback( p_filter, CFG_PREFIX "mask", EraseCallback, p_sys );
    vlc_mutex_destroy( &p_sys->lock );
    free( p_filter->p_sys );
}
