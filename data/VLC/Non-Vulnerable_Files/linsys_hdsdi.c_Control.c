 *****************************************************************************/
static int Control( demux_t *p_demux, int i_query, va_list args )
{
    bool *pb;
    int64_t *pi64;
    switch( i_query )
    {
        /* Special for access_demux */
        case DEMUX_CAN_PAUSE:
        case DEMUX_CAN_CONTROL_PACE:
            /* TODO */
            pb = (bool*)va_arg( args, bool * );
            *pb = false;
            return VLC_SUCCESS;
        case DEMUX_GET_PTS_DELAY:
            pi64 = (int64_t*)va_arg( args, int64_t * );
            *pi64 = INT64_C(1000)
                  * var_InheritInteger( p_demux, "live-caching" );
            return VLC_SUCCESS;
        /* TODO implement others */
        default:
            return VLC_EGENERIC;
    }
}
