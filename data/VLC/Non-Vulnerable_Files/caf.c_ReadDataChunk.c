}
static int ReadDataChunk( demux_t *p_demux, uint64_t i_size )
{
    if( i_size < 4 )
        return VLC_EGENERIC;
    demux_sys_t *p_sys = p_demux->p_sys;
    p_sys->i_data_offset = stream_Tell( p_demux->s ) + 4; /* skip edit count */
    p_sys->i_data_size = i_size == kCHUNK_SIZE_EOF ? kCHUNK_SIZE_EOF : ( i_size - 4 );
    return VLC_SUCCESS;
}
