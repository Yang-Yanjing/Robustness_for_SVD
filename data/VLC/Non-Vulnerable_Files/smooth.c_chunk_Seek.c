 * be needed anymore (among others). */
static int chunk_Seek( stream_t *s, const uint64_t pos )
{
    stream_sys_t *p_sys = s->p_sys;
    if( pos == p_sys->playback.boffset )
        return VLC_SUCCESS;
    chunk_t *chunk = get_chunk( s, false );
    if( chunk == NULL )
        return VLC_EGENERIC;
    bool inside_chunk = pos >= chunk->offset &&
            pos < (chunk->offset + chunk->size) ? true : false;
    if( inside_chunk )
    {
        chunk->read_pos = pos - chunk->offset;
        p_sys->playback.boffset = pos;
        return VLC_SUCCESS;
    }
    else
    {
        if( p_sys->b_live )
        {
            msg_Err( s, "Cannot seek outside the current chunk for a live stream" );
            return VLC_EGENERIC;
        }
        msg_Info( s, "Seeking outside the current chunk" );
        assert( pos <= FAKE_STREAM_SIZE );
        vlc_mutex_lock( &p_sys->download.lock_wait );
        p_sys->b_tseek = true;
        p_sys->time_pos = p_sys->vod_duration * pos / FAKE_STREAM_SIZE;
        for( int i = 0; i < 3; i++ )
            p_sys->download.lead[i] = 0;
        p_sys->playback.toffset = 0;
        vlc_cond_signal( &p_sys->download.wait);
        vlc_mutex_unlock( &p_sys->download.lock_wait );
        return VLC_SUCCESS;
    }
}
