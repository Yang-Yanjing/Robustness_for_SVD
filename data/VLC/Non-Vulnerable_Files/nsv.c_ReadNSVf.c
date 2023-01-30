 *****************************************************************************/
static int ReadNSVf( demux_t *p_demux )
{
    /* demux_sys_t *p_sys = p_demux->p_sys; */
    const uint8_t     *p;
    msg_Dbg( p_demux, "new NSVf chunk" );
    if( stream_Peek( p_demux->s, &p, 8 ) < 8 )
    {
        return VLC_EGENERIC;
    }
    uint32_t i_header_size = GetDWLE( &p[4] );
    msg_Dbg( p_demux, "    - size=%" PRIu32, i_header_size );
    if( i_header_size == 0 || i_header_size == UINT32_MAX )
        return VLC_EGENERIC;
    return stream_Read( p_demux->s, NULL, i_header_size ) == i_header_size ? VLC_SUCCESS : VLC_EGENERIC;
}
