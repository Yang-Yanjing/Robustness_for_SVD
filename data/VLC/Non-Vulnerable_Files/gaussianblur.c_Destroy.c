}
static void Destroy( vlc_object_t *p_this )
{
    filter_t *p_filter = (filter_t *)p_this;
    free( p_filter->p_sys->pt_distribution );
    free( p_filter->p_sys->pt_buffer );
    free( p_filter->p_sys->pt_scale );
    free( p_filter->p_sys );
}
