 *****************************************************************************/
static void ClosePostproc( vlc_object_t *p_this )
{
    filter_t *p_filter = (filter_t *)p_this;
    filter_sys_t *p_sys = p_filter->p_sys;
    /* delete the callback before destroying the mutex */
    var_DelCallback( p_filter, FILTER_PREFIX "q", PPQCallback, NULL );
    var_DelCallback( p_filter, FILTER_PREFIX "name", PPNameCallback, NULL );
    /* Destroy the resources */
    vlc_mutex_destroy( &p_sys->lock );
    pp_free_context( p_sys->pp_context );
    if( p_sys->pp_mode ) pp_free_mode( p_sys->pp_mode );
    free( p_sys );
}
