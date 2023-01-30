}
static int Control(access_t *access, int query, va_list args)
{
    access_sys_t *sys = access->p_sys;
    bool *b;
    switch (query) {
    case ACCESS_CAN_SEEK:
    case ACCESS_CAN_FASTSEEK: /* FIXME how to do that ? */
        b = va_arg(args, bool *);
#if LIBAVFORMAT_VERSION_MAJOR < 54
        *b = !sys->context->is_streamed;
#else
        *b = sys->context->seekable;
#endif
        return VLC_SUCCESS;
    case ACCESS_CAN_PAUSE:
        b = va_arg(args, bool *);
#if LIBAVFORMAT_VERSION_MAJOR < 54
        *b = sys->context->prot->url_read_pause != NULL;
#else
        *b = sys->context->read_pause != NULL;
#endif
        return VLC_SUCCESS;
    case ACCESS_CAN_CONTROL_PACE:
        b = va_arg(args, bool *);
        *b = true; /* FIXME */
        return VLC_SUCCESS;
    case ACCESS_GET_SIZE:
        *va_arg(args, uint64_t *) = sys->size;
        return VLC_SUCCESS;
    case ACCESS_GET_PTS_DELAY: {
        int64_t *delay = va_arg(args, int64_t *);
        *delay = INT64_C(1000) * var_InheritInteger(access, "network-caching");
        return VLC_SUCCESS;
    }
    case ACCESS_SET_PAUSE_STATE: {
        bool is_paused = va_arg(args, int);
        if (avio_pause(sys->context, is_paused)< 0)
            return VLC_EGENERIC;
        return VLC_SUCCESS;
    }
    default:
        return VLC_EGENERIC;
    }
}
