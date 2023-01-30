}
static void CloseEncoder( vlc_object_t *p_this )
{
    encoder_sys_t *p_sys = ((encoder_t*)p_this)->p_sys;
    vlc_mutex_lock( &entrant.lock );
    entrant.busy = false;
    vlc_mutex_unlock( &entrant.lock );
    /* TODO: we should send the last PCM block padded with 0
     * But we don't know if other blocks will come before it's too late */
    if( p_sys->i_buffer )
        free( p_sys->p_buffer );
    shine_close(p_sys->s);
    block_FifoRelease( p_sys->p_fifo );
    free( p_sys );
}
