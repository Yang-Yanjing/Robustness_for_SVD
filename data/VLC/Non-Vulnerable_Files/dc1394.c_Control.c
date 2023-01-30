 *****************************************************************************/
static int Control( demux_t *p_demux, int i_query, va_list args )
{
    VLC_UNUSED( p_demux );
    switch( i_query )
    {
        /* Special for access_demux */
        case DEMUX_CAN_PAUSE:
        case DEMUX_CAN_SEEK:
        case DEMUX_SET_PAUSE_STATE:
        case DEMUX_CAN_CONTROL_PACE:
            *va_arg( args, bool * ) = false;
            return VLC_SUCCESS;
        case DEMUX_GET_PTS_DELAY:
            *va_arg( args, int64_t * ) = (int64_t)DEFAULT_PTS_DELAY;
            return VLC_SUCCESS;
        case DEMUX_GET_TIME:
            *va_arg( args, int64_t * ) = mdate();
            return VLC_SUCCESS;
        /* TODO implement others */
        default:
            return VLC_EGENERIC;
    }
    return VLC_EGENERIC;
}
