/* The MP4 demux should never have to to peek outside the current chunk */
static int Peek( stream_t *s, const uint8_t **pp_peek, unsigned i_peek )
{
    chunk_t *chunk = get_chunk( s, true );
    if( !chunk || !chunk->data )
        return 0;
    int bytes = chunk->size - chunk->read_pos;
    assert( bytes > 0 );
    if( (unsigned)bytes < i_peek )
    {
        msg_Err( s, "could not peek %u bytes, only %i!", i_peek, bytes );
    }
    msg_Dbg( s, "peeking at chunk %u!", chunk->sequence );
    *pp_peek = chunk->data + chunk->read_pos;
    return bytes;
}
