}
static bool TsIsUnused( ts_thread_t *p_ts )
{
    bool b_unused;
    vlc_mutex_lock( &p_ts->lock );
    b_unused = !p_ts->b_paused &&
               p_ts->i_rate == p_ts->i_rate_source &&
               TsStorageIsEmpty( p_ts->p_storage_r );
    vlc_mutex_unlock( &p_ts->lock );
    return b_unused;
}
