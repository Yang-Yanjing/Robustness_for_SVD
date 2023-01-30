 *****************************************************************************/
static int Control (demux_t *demux, int i_query, va_list args)
{
    demux_sys_t *sys = demux->p_sys;
    switch (i_query)
    {
        case DEMUX_GET_POSITION:
            if (!sys->duration)
                return VLC_EGENERIC;
            *va_arg (args, double *) = (sys->tick - (double)VLC_TS_0)
                                     / sys->duration;
            break;
        case DEMUX_SET_POSITION:
            return Seek (demux, va_arg (args, double) * sys->duration);
        case DEMUX_GET_LENGTH:
            *va_arg (args, int64_t *) = sys->duration;
            break;
        case DEMUX_GET_TIME:
            *va_arg (args, int64_t *) = sys->tick - VLC_TS_0;
            break;
        case DEMUX_SET_TIME:
            return Seek (demux, va_arg (args, int64_t));
        default:
            return VLC_EGENERIC;
    }
    return VLC_SUCCESS;
}
