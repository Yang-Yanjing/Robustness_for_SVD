}
static void Destroy( vlc_object_t *p_this )
{
    filter_t *p_filter = (filter_t *)p_this;
    filter_sys_t *p_sys = p_filter->p_sys;
    var_DelCallback( p_filter, CFG_PREFIX "u", BluescreenCallback, p_sys );
    var_DelCallback( p_filter, CFG_PREFIX "v", BluescreenCallback, p_sys );
    var_DelCallback( p_filter, CFG_PREFIX "ut", BluescreenCallback, p_sys );
    var_DelCallback( p_filter, CFG_PREFIX "vt", BluescreenCallback, p_sys );
    free( p_sys->p_at );
    vlc_mutex_destroy( &p_sys->lock );
    free( p_sys );
}
