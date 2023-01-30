 */
static int NextChunk( demux_t *p_demux, vlc_fourcc_t *p_fcc, uint64_t *pi_size )
{
    uint8_t p_read[12];
    if( stream_Read( p_demux->s, p_read, 12 ) < 12 )
        return VLC_EGENERIC;
    *p_fcc = ReadFOURCC( p_read );
    uint64_t i_size = GetQWBE( p_read + 4 );
    /* We accept no negativ sizes for chunks, except -1 for the data chunk. */
    if( i_size > INT64_MAX )
    {
        if( *p_fcc == VLC_FOURCC( 'd', 'a', 't', 'a' ) && i_size == UINT64_C( -1 ))
            i_size = kCHUNK_SIZE_EOF;
        else
            return VLC_EGENERIC;
    }
    *pi_size = i_size;
    return VLC_SUCCESS;
}
