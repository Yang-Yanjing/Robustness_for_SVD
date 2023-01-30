}
static bool SkipAPETag( demux_t *p_demux )
{
    const uint8_t *p_peek;
    int i_version;
    int i_size;
    uint32_t flags;
    if( !p_demux->s )
        return false;
    /* Get 32 byte ape header */
    if( stream_Peek( p_demux->s, &p_peek, 32 ) < 32 )
        return false;
    if( memcmp( p_peek, "APETAGEX", 8 ) )
        return false;
    i_version = GetDWLE( &p_peek[8] );
    flags = GetDWLE( &p_peek[8+4+4] );
    if( ( i_version != 1000 && i_version != 2000 ) || !( flags & (1<<29) ) )
        return false;
    i_size = GetDWLE( &p_peek[8+4] ) + ( (flags&(1<<30)) ? 32 : 0 );
    /* Skip the entire tag */
    if( stream_Read( p_demux->s, NULL, i_size ) < i_size )
        return false;
    msg_Dbg( p_demux, "AP2 v%d tag found, skipping %d bytes",
             i_version/1000, i_size );
    return true;
}
