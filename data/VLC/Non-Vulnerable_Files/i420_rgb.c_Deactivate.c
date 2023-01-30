 *****************************************************************************/
static void Deactivate( vlc_object_t *p_this )
{
    filter_t *p_filter = (filter_t *)p_this;
#ifdef PLAIN
    free( p_filter->p_sys->p_base );
#endif
    free( p_filter->p_sys->p_offset );
    free( p_filter->p_sys->p_buffer );
    free( p_filter->p_sys );
}
