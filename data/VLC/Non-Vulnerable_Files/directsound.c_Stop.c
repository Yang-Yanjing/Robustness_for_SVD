 */
static HRESULT Stop( aout_stream_sys_t *p_sys )
{
    vlc_mutex_lock( &p_sys->lock );
    p_sys->b_playing =  true;
    vlc_cond_signal( &p_sys->cond );
    vlc_mutex_unlock( &p_sys->lock );
    vlc_cancel( p_sys->eraser_thread );
    vlc_join( p_sys->eraser_thread, NULL );
    vlc_cond_destroy( &p_sys->cond );
    vlc_mutex_destroy( &p_sys->lock );
    if( p_sys->p_notify != NULL )
    {
        IDirectSoundNotify_Release(p_sys->p_notify );
        p_sys->p_notify = NULL;
    }
    if( p_sys->p_dsbuffer != NULL )
    {
        IDirectSoundBuffer_Stop( p_sys->p_dsbuffer );
        IDirectSoundBuffer_Release( p_sys->p_dsbuffer );
        p_sys->p_dsbuffer = NULL;
    }
    if( p_sys->p_dsobject != NULL )
    {
        IDirectSound_Release( p_sys->p_dsobject );
        p_sys->p_dsobject = NULL;
    }
    return DS_OK;
}
