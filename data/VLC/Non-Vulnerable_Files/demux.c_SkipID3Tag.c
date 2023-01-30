}
static bool SkipID3Tag( demux_t *p_demux )
{
    const uint8_t *p_peek;
    uint8_t version, revision;
    int i_size;
    int b_footer;
    if( !p_demux->s )
        return false;
    /* Get 10 byte id3 header */
    if( stream_Peek( p_demux->s, &p_peek, 10 ) < 10 )
        return false;
    if( memcmp( p_peek, "ID3", 3 ) )
        return false;
    version = p_peek[3];
    revision = p_peek[4];
    b_footer = p_peek[5] & 0x10;
    i_size = (p_peek[6]<<21) + (p_peek[7]<<14) + (p_peek[8]<<7) + p_peek[9];
    if( b_footer ) i_size += 10;
    i_size += 10;
    /* Skip the entire tag */
    if( stream_Read( p_demux->s, NULL, i_size ) < i_size )
        return false;
    msg_Dbg( p_demux, "ID3v2.%d revision %d tag found, skipping %d bytes",
             version, revision, i_size );
    return true;
}
