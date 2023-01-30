 */
static int ControlAccess(access_t *access, int i_query, va_list args)
{
    (void) access;
    switch (i_query)
    {
    case ACCESS_CAN_SEEK:
    case ACCESS_CAN_FASTSEEK: {
        bool *b = va_arg( args, bool* );
        *b = false;
        return VLC_SUCCESS;
    }
    case ACCESS_CAN_PAUSE:
    case ACCESS_CAN_CONTROL_PACE: {
        bool *b = va_arg( args, bool* );
        *b = true;
        return VLC_SUCCESS;
    }
    case ACCESS_GET_SIZE: {
        uint64_t *s = va_arg(args, uint64_t *);
        *s = var_InheritInteger(access, "imem-size");
        return VLC_SUCCESS;
    }
    case ACCESS_GET_PTS_DELAY: {
        int64_t *delay = va_arg(args, int64_t *);
        *delay = DEFAULT_PTS_DELAY; /* FIXME? */
        return VLC_SUCCESS;
    }
    case ACCESS_SET_PAUSE_STATE:
        return VLC_SUCCESS;
    default:
        return VLC_EGENERIC;
    }
}
