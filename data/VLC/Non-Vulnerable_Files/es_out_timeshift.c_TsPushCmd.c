}
static void TsPushCmd( ts_thread_t *p_ts, ts_cmd_t *p_cmd )
{
    vlc_mutex_lock( &p_ts->lock );
    if( !p_ts->p_storage_w || TsStorageIsFull( p_ts->p_storage_w, p_cmd ) )
    {
        ts_storage_t *p_storage = TsStorageNew( p_ts->psz_tmp_path, p_ts->i_tmp_size_max );
        if( !p_storage )
        {
            CmdClean( p_cmd );
            vlc_mutex_unlock( &p_ts->lock );
            /* TODO warn the user (but only once) */
            return;
        }
        if( !p_ts->p_storage_w )
        {
            p_ts->p_storage_r = p_ts->p_storage_w = p_storage;
        }
        else
        {
            TsStoragePack( p_ts->p_storage_w );
            p_ts->p_storage_w->p_next = p_storage;
            p_ts->p_storage_w = p_storage;
        }
    }
    /* TODO return error and warn the user (but only once) */
    TsStoragePushCmd( p_ts->p_storage_w, p_cmd, p_ts->p_storage_r == p_ts->p_storage_w );
    vlc_cond_signal( &p_ts->wait );
    vlc_mutex_unlock( &p_ts->lock );
}
