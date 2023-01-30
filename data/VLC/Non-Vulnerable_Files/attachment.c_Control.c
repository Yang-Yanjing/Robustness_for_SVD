/* */
static int Control(access_t *access, int query, va_list args)
{
    VLC_UNUSED(access);
    switch (query)
    {
    /* */
    case ACCESS_CAN_SEEK:
    case ACCESS_CAN_FASTSEEK:
    case ACCESS_CAN_PAUSE:
    case ACCESS_CAN_CONTROL_PACE: {
        bool *b = va_arg(args, bool*);
        *b = true;
        return VLC_SUCCESS;
    }
    case ACCESS_GET_PTS_DELAY: {
        int64_t *d = va_arg(args, int64_t *);
        *d = DEFAULT_PTS_DELAY;
        return VLC_SUCCESS;
    }
    case ACCESS_SET_PAUSE_STATE:
        return VLC_SUCCESS;
    default:
        return VLC_EGENERIC;
    }
}
