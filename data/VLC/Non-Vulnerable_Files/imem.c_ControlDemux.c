 */
static int ControlDemux(demux_t *demux, int i_query, va_list args)
{
    imem_sys_t *sys = (imem_sys_t*)demux->p_sys;
    switch (i_query)
    {
    case DEMUX_CAN_PAUSE:
    case DEMUX_CAN_CONTROL_PACE: {
        bool *b = va_arg(args, bool *);
        *b = true;
        return VLC_SUCCESS;
    }
    case DEMUX_SET_PAUSE_STATE:
        return VLC_SUCCESS;
    case DEMUX_GET_PTS_DELAY: {
        int64_t *delay = va_arg(args, int64_t *);
        *delay = DEFAULT_PTS_DELAY; /* FIXME? */
        return VLC_SUCCESS;
    }
    case DEMUX_GET_POSITION: {
        double *position = va_arg(args, double *);
        *position = 0.0;
        return VLC_SUCCESS;
    }
    case DEMUX_GET_TIME: {
        int64_t *t = va_arg(args, int64_t *);
        *t = sys->dts;
        return VLC_SUCCESS;
    }
    case DEMUX_GET_LENGTH: {
        int64_t *l = va_arg(args, int64_t *);
        *l = 0;
        return VLC_SUCCESS;
    }
    case DEMUX_SET_NEXT_DEMUX_TIME:
        sys->deadline = va_arg(args, int64_t);
        return VLC_SUCCESS;
    /* */
    case DEMUX_CAN_SEEK:
    case DEMUX_SET_POSITION:
    case DEMUX_SET_TIME:
    default:
        return VLC_EGENERIC;
    }
    return VLC_EGENERIC;
}
