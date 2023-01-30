}
static int TsPopCmdLocked( ts_thread_t *p_ts, ts_cmd_t *p_cmd, bool b_flush )
{
    vlc_assert_locked( &p_ts->lock );
    if( TsStorageIsEmpty( p_ts->p_storage_r ) )
        return VLC_EGENERIC;
    TsStoragePopCmd( p_ts->p_storage_r, p_cmd, b_flush );
    while( p_ts->p_storage_r && TsStorageIsEmpty( p_ts->p_storage_r ) )
    {
        ts_storage_t *p_next = p_ts->p_storage_r->p_next;
        if( !p_next )
            break;
        TsStorageDelete( p_ts->p_storage_r );
        p_ts->p_storage_r = p_next;
    }
    return VLC_SUCCESS;
}
