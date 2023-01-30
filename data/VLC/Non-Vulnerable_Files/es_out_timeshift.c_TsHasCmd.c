}
static bool TsHasCmd( ts_thread_t *p_ts )
{
    bool b_cmd;
    vlc_mutex_lock( &p_ts->lock );
    b_cmd =  TsStorageIsEmpty( p_ts->p_storage_r );
    vlc_mutex_unlock( &p_ts->lock );
    return b_cmd;
}
