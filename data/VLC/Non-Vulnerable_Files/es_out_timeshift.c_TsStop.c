}
static void TsStop( ts_thread_t *p_ts )
{
    vlc_cancel( p_ts->thread );
    vlc_join( p_ts->thread, NULL );
    vlc_mutex_lock( &p_ts->lock );
    for( ;; )
    {
        ts_cmd_t cmd;
        if( TsPopCmdLocked( p_ts, &cmd, true ) )
            break;
        CmdClean( &cmd );
    }
    assert( !p_ts->p_storage_r || !p_ts->p_storage_r->p_next );
    if( p_ts->p_storage_r )
        TsStorageDelete( p_ts->p_storage_r );
    vlc_mutex_unlock( &p_ts->lock );
    TsDestroy( p_ts );
}
