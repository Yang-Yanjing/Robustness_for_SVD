 */
static int Control (demux_t *demux, int query, va_list args)
{
    switch (query)
    {
        case DEMUX_GET_POSITION:
        {
            float *v = va_arg (args, float *);
            *v = 0.;
            return VLC_SUCCESS;
        }
        case DEMUX_GET_LENGTH:
        case DEMUX_GET_TIME:
        {
            int64_t *v = va_arg (args, int64_t *);
            *v = 0;
            return VLC_SUCCESS;
        }
        case DEMUX_GET_PTS_DELAY:
        {
            int64_t *v = va_arg (args, int64_t *);
            *v = INT64_C(1000) * var_InheritInteger (demux, "live-caching");
            return VLC_SUCCESS;
        }
        case DEMUX_CAN_PAUSE:
        case DEMUX_CAN_CONTROL_PACE:
        case DEMUX_CAN_CONTROL_RATE:
        case DEMUX_CAN_SEEK:
        {
            bool *v = va_arg (args, bool *);
            *v = false;
            return VLC_SUCCESS;
        }
        case DEMUX_SET_PAUSE_STATE:
            return VLC_SUCCESS; /* should not happen */
    }
    return VLC_EGENERIC;
}
