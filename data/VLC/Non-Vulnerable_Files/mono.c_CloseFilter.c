 *****************************************************************************/
static void CloseFilter( vlc_object_t *p_this)
{
    filter_t *p_filter = (filter_t *) p_this;
    filter_sys_t *p_sys = p_filter->p_sys;
    free( p_sys->p_atomic_operations );
    free( p_sys->p_overflow_buffer );
    free( p_sys );
}
