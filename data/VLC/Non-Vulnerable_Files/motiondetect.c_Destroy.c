 *****************************************************************************/
static void Destroy( vlc_object_t *p_this )
{
    filter_t *p_filter = (filter_t *)p_this;
    filter_sys_t *p_sys = p_filter->p_sys;
    free( p_sys->p_buf2 );
    free( p_sys->p_buf );
    picture_Release( p_sys->p_old );
    free( p_sys );
}
