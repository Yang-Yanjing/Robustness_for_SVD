}
static int DemuxControl( demux_t *p_demux, int i_query, va_list args )
{
    (void)p_demux; (void)i_query; (void)args;
    switch( i_query )
    {
    case DEMUX_GET_PTS_DELAY:
    {
        int64_t *pi_pts_delay = va_arg( args, int64_t * );
        *pi_pts_delay = DEFAULT_PTS_DELAY;
        return VLC_SUCCESS;
    }
    default:
        return VLC_EGENERIC;
    }
}
