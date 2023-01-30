 *****************************************************************************/
static bool Peek( demux_t *p_demux, bool b_first )
{
    int i_data;
    demux_sys_t *p_sys = p_demux->p_sys;
    if( b_first )
    {
        p_sys->i_data_peeked = 0;
    }
    else if( p_sys->i_data_peeked == p_sys->i_frame_size_estimate )
    {
        p_sys->i_frame_size_estimate += 5120;
    }
    i_data = stream_Peek( p_demux->s, &p_sys->p_peek,
                          p_sys->i_frame_size_estimate );
    if( i_data == p_sys->i_data_peeked )
    {
        msg_Warn( p_demux, "no more data" );
        return false;
    }
    p_sys->i_data_peeked = i_data;
    if( i_data <= 0 )
    {
        msg_Warn( p_demux, "cannot peek data" );
        return false;
    }
    return true;
}
