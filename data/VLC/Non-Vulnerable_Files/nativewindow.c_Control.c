 */
static int Control(vout_window_t *wnd, int cmd, va_list ap)
{
    switch (cmd)
    {
        case VOUT_WINDOW_SET_SIZE:
        {
            unsigned width = va_arg(ap, unsigned);
            unsigned height = va_arg(ap, unsigned);
            jni_SetAndroidSurfaceSize(width, height, width, height, 1, 1);
            break;
        }
        case VOUT_WINDOW_SET_STATE:
        case VOUT_WINDOW_SET_FULLSCREEN:
            return VLC_EGENERIC;
        default:
            msg_Err (wnd, "request %d not implemented", cmd);
            return VLC_EGENERIC;
    }
    return VLC_SUCCESS;
}
