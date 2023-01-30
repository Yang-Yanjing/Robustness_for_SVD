 *****************************************************************************/
static void DestroyFilter( vlc_object_t *p_this )
{
    filter_t *p_filter = (filter_t *)p_this;
    filter_sys_t *p_sys = p_filter->p_sys;
    vlc_timer_destroy( p_sys->timer );
    vlc_mutex_destroy( &p_sys->lock );
    text_style_Delete( p_sys->p_style );
    free( p_sys->psz_marquee );
    FreeRSS( p_sys->p_feeds, p_sys->i_feeds );
    free( p_sys );
}
