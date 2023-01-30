 */
static void Close( vlc_object_t *p_this )
{
    filter_t *p_filter = (filter_t *)p_this;
    filter_sys_t *p_sys = p_filter->p_sys;
    for( int i = 0; ppsz_filter_callbacks[i]; i++ )
        var_DelCallback( p_filter, ppsz_filter_callbacks[i],
                         LogoCallback, p_sys );
    if( p_sys->p_blend )
        filter_DeleteBlend( p_sys->p_blend );
    vlc_mutex_destroy( &p_sys->lock );
    LogoListUnload( &p_sys->list );
    free( p_sys );
}
