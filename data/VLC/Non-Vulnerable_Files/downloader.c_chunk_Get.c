}
static chunk_t * chunk_Get( sms_stream_t *sms, const int64_t start_time )
{
    int len = vlc_array_count( sms->chunks );
    for( int i = 0; i < len; i++ )
    {
        chunk_t * chunk = vlc_array_item_at_index( sms->chunks, i );
        if( !chunk ) return NULL;
        if( chunk->start_time <= start_time &&
                chunk->start_time + chunk->duration > start_time )
        {
            return chunk;
        }
    }
    return NULL;
}
