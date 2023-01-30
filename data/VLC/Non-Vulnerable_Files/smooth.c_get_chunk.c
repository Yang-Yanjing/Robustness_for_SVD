}
static chunk_t *get_chunk( stream_t *s, const bool wait )
{
    stream_sys_t *p_sys = s->p_sys;
    unsigned count;
    chunk_t *chunk = NULL;
    vlc_mutex_lock( &p_sys->download.lock_wait );
    count = vlc_array_count( p_sys->download.chunks );
    while( p_sys->playback.index >= count || p_sys->b_tseek )
    {
        /* Yes I know, checking for p_sys->b_die is not reliable,
         * that's why vlc_object_alive() has been deprecated. But if I
         * understood well, there is no good solution with a stream_filter
         * module anyaway. */
        if( !wait || p_sys->b_error )
        {
            vlc_mutex_unlock( &p_sys->download.lock_wait );
            msg_Warn( s, "get_chunk failed! (playback index %u)",
                    p_sys->playback.index );
            return NULL;
        }
        if( NO_MORE_CHUNKS )
        {
            vlc_mutex_unlock( &p_sys->download.lock_wait );
            msg_Info( s, "No more chunks, end of the VOD" );
            return NULL;
        }
        msg_Dbg( s, "get_chunk is waiting !!!" );
        vlc_cond_timedwait( &p_sys->download.wait,
                &p_sys->download.lock_wait, mdate() + 500000 );
        count = vlc_array_count( p_sys->download.chunks );
        msg_Dbg( s, "count is %u, and index is %u", count, p_sys->playback.index );
    }
    chunk = vlc_array_item_at_index( p_sys->download.chunks, p_sys->playback.index );
    vlc_mutex_unlock( &p_sys->download.lock_wait );
    return chunk;
}
