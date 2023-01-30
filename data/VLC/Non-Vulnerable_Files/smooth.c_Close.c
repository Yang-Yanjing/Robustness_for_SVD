}
static void Close( vlc_object_t *p_this )
{
    stream_t *s = (stream_t*)p_this;
    stream_sys_t *p_sys = s->p_sys;
    vlc_mutex_lock( &p_sys->download.lock_wait );
    p_sys->b_close = true;
    /* Negate the condition variable's predicate */
    for( int i = 0; i < 3; i++ )
        p_sys->download.lead[i] = 0;
    p_sys->playback.toffset = 0;
    vlc_cond_signal(&p_sys->download.wait);
    vlc_mutex_unlock( &p_sys->download.lock_wait );
    vlc_join( p_sys->thread, NULL );
    vlc_mutex_destroy( &p_sys->download.lock_wait );
    vlc_cond_destroy( &p_sys->download.wait );
    SysCleanup( p_sys );
    free( p_sys );
}
