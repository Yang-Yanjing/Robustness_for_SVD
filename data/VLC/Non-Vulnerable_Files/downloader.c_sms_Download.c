}
static int sms_Download( stream_t *s, chunk_t *chunk, char *url )
{
    stream_sys_t *p_sys = s->p_sys;
    stream_t *p_ts = stream_UrlNew( s, url );
    free( url );
    if( p_ts == NULL )
        return VLC_EGENERIC;
    int64_t size = stream_Size( p_ts );
    chunk->size = size;
    chunk->offset = p_sys->download.next_chunk_offset;
    p_sys->download.next_chunk_offset += chunk->size;
    chunk->data = malloc( size );
    if( chunk->data == NULL )
    {
        stream_Delete( p_ts );
        return VLC_ENOMEM;
    }
    int read = stream_Read( p_ts, chunk->data, size );
    if( read < size )
    {
        msg_Warn( s, "sms_Download: I requested %"PRIi64" bytes, "\
                "but I got only %i", size, read );
        chunk->data = realloc( chunk->data, read );
    }
    stream_Delete( p_ts );
    vlc_mutex_lock( &p_sys->download.lock_wait );
    int index = es_cat_to_index( chunk->type );
    p_sys->download.lead[index] += chunk->duration;
    vlc_mutex_unlock( &p_sys->download.lock_wait );
    return VLC_SUCCESS;
}
