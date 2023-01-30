};
static int RadioControl (demux_t *demux, int query, va_list args)
{
    demux_sys_t *sys = demux->p_sys;
    switch (query)
    {
        case DEMUX_CAN_PAUSE:
        case DEMUX_CAN_SEEK:
        case DEMUX_CAN_CONTROL_PACE:
            *va_arg (args, bool *) = false;
            break;
        case DEMUX_GET_PTS_DELAY:
            *va_arg (args,int64_t *) = INT64_C(1000)
                * var_InheritInteger (demux, "live-caching");
            break;
        case DEMUX_GET_TIME:
            *va_arg (args, int64_t *) = mdate () - sys->start;
            break;
        /* TODO implement others */
        default:
            return VLC_EGENERIC;
    }
    return VLC_SUCCESS;
}
