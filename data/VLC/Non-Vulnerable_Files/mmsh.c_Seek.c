 *****************************************************************************/
static int Seek( access_t *p_access, uint64_t i_pos )
{
    access_sys_t *p_sys = p_access->p_sys;
    chunk_t      ck;
    uint64_t     i_offset;
    uint64_t     i_packet;
    msg_Dbg( p_access, "seeking to %"PRId64, i_pos );
    i_packet = ( i_pos - p_sys->i_header ) / p_sys->asfh.i_min_data_packet_size;
    i_offset = ( i_pos - p_sys->i_header ) % p_sys->asfh.i_min_data_packet_size;
    Stop( p_access );
    Start( p_access, i_packet * p_sys->asfh.i_min_data_packet_size );
    while( vlc_object_alive (p_access) )
    {
        if( GetPacket( p_access, &ck ) )
            break;
        /* skip headers */
        if( ck.i_type != 0x4824 )
            break;
        msg_Warn( p_access, "skipping header" );
    }
    p_access->info.i_pos = i_pos;
    p_access->info.b_eof = false;
    p_sys->i_packet_used += i_offset;
    return VLC_SUCCESS;
}
