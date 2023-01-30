 *****************************************************************************/
static int MjpgDemux( demux_t *p_demux )
{
    demux_sys_t *p_sys = p_demux->p_sys;
    int i;
    if( p_sys->b_still && p_sys->i_still_end )
    {
        /* Still frame, wait until the pause delay is gone */
        mwait( p_sys->i_still_end );
        p_sys->i_still_end = 0;
        return 1;
    }
    if( !Peek( p_demux, true ) )
    {
        msg_Warn( p_demux, "cannot peek data" );
        return 0;
    }
    if( p_sys->i_data_peeked < 4 )
    {
        msg_Warn( p_demux, "data shortage" );
        return 0;
    }
    i = 3;
FIND_NEXT_EOI:
    while( !( p_sys->p_peek[i-1] == 0xFF && p_sys->p_peek[i] == 0xD9 ) )
    {
        if( p_sys->p_peek[i-1] == 0xFF && p_sys->p_peek[i] == 0xD9  )
        {
            p_sys->i_level++;
            msg_Dbg( p_demux, "we found another JPEG SOI at %d", i );
        }
        i++;
        if( i >= p_sys->i_data_peeked )
        {
            msg_Dbg( p_demux, "did not find JPEG EOI in %d bytes",
                     p_sys->i_data_peeked );
            if( !Peek( p_demux, false ) )
            {
                msg_Warn( p_demux, "no more data is available at the moment" );
                return 0;
            }
        }
    }
    i++;
    msg_Dbg( p_demux, "JPEG EOI detected at %d", i );
    p_sys->i_level--;
    if( p_sys->i_level > 0 )
        goto FIND_NEXT_EOI;
    return SendBlock( p_demux, i );
}
