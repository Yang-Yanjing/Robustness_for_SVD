}
static void SplitterEvent(vout_display_t *vd, int event, va_list args)
{
    //vout_display_owner_sys_t *osys = vd->owner.sys;
    switch (event) {
#if 0
    case VOUT_DISPLAY_EVENT_MOUSE_STATE:
    case VOUT_DISPLAY_EVENT_MOUSE_MOVED:
    case VOUT_DISPLAY_EVENT_MOUSE_PRESSED:
    case VOUT_DISPLAY_EVENT_MOUSE_RELEASED:
        /* TODO */
        break;
#endif
    case VOUT_DISPLAY_EVENT_MOUSE_DOUBLE_CLICK:
    case VOUT_DISPLAY_EVENT_KEY:
    case VOUT_DISPLAY_EVENT_CLOSE:
    case VOUT_DISPLAY_EVENT_FULLSCREEN:
    case VOUT_DISPLAY_EVENT_DISPLAY_SIZE:
    case VOUT_DISPLAY_EVENT_PICTURES_INVALID:
        VoutDisplayEvent(vd, event, args);
        break;
    default:
        msg_Err(vd, "splitter event not implemented: %d", event);
        break;
    }
}
