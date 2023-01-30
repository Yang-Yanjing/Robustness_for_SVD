}
static void DecSysRelease( decoder_sys_t *p_sys )
{
    /* */
    vlc_mutex_lock( &p_sys->lock );
    p_sys->i_refcount--;
    if( p_sys->i_refcount > 0 )
    {
        vlc_mutex_unlock( &p_sys->lock );
        return;
    }
    vlc_mutex_unlock( &p_sys->lock );
    vlc_mutex_destroy( &p_sys->lock );
    if( p_sys->p_track )
        ass_free_track( p_sys->p_track );
    if( p_sys->p_renderer )
        ass_renderer_done( p_sys->p_renderer );
    if( p_sys->p_library )
        ass_library_done( p_sys->p_library );
    free( p_sys );
}
