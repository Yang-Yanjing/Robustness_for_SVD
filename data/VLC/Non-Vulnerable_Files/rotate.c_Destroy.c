 *****************************************************************************/
static void Destroy( vlc_object_t *p_this )
{
    filter_t *p_filter = (filter_t *)p_this;
    filter_sys_t *p_sys = p_filter->p_sys;
    if( p_sys->p_motion != NULL )
        motion_destroy( p_sys->p_motion );
    else
    {
        var_DelCallback( p_filter, FILTER_PREFIX "angle",
                         RotateCallback, p_sys );
    }
    free( p_sys );
}
