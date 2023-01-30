}
static void OggGetSkeletonIndex( uint8_t **pp_buffer, long *pi_size, ogg_stream_t *p_stream )
{
    uint8_t *p_buffer = calloc( INDEX_BASE_SIZE + p_stream->skeleton.i_index_size, sizeof(uint8_t) );
    if ( !p_buffer ) return;
    *pp_buffer = p_buffer;
    memcpy( p_buffer, "index", 6 );
    SetDWLE( &p_buffer[6], p_stream->i_serial_no );
    SetQWLE( &p_buffer[10], p_stream->skeleton.i_index_count ); /* num keypoints */
    SetQWLE( &p_buffer[18], 1000000 );
    SetQWLE( &p_buffer[34], p_stream->i_length );
    memcpy( p_buffer + INDEX_BASE_SIZE, p_stream->skeleton.p_index, p_stream->skeleton.i_index_payload );
    *pi_size = INDEX_BASE_SIZE + p_stream->skeleton.i_index_size;
}
