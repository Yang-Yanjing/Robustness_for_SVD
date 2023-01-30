}
static void Close( vlc_object_t *p_this )
{
    filter_t *p_filter = (filter_t *)p_this;
    free( p_filter->p_sys->p_state );
    free( p_filter->p_sys );
}
