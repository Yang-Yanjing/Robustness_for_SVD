}
static void Close( vlc_object_t *p_this )
{
    filter_t *p_filter = (filter_t *)p_this;
    filter_sys_t *p_sys = p_filter->p_sys;
    free( p_sys->buf_queue );
    free( p_sys->buf_overlap );
    free( p_sys->table_blend );
    free( p_sys->buf_pre_corr );
    free( p_sys->table_window );
    free( p_sys );
}
