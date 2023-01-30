}
static int Control (vout_window_t *wnd, int query, va_list ap)
{
    VLC_UNUSED( ap );
    switch (query)
    {
        case VOUT_WINDOW_SET_SIZE:   /* not allowed */
        case VOUT_WINDOW_SET_STATE: /* not allowed either, would be ugly */
            return VLC_EGENERIC;
        default:
            msg_Warn (wnd, "unsupported control query %d", query);
            return VLC_EGENERIC;
    }
}
