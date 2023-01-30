}
static int query_resolution(vout_display_t *vd, unsigned *width, unsigned *height)
{
    TV_DISPLAY_STATE_T display_state;
    int ret = 0;
    if (vc_tv_get_display_state(&display_state) == 0) {
        if (display_state.state & 0xFF) {
            *width = display_state.display.hdmi.width;
            *height = display_state.display.hdmi.height;
        } else if (display_state.state & 0xFF00) {
            *width = display_state.display.sdtv.width;
            *height = display_state.display.sdtv.height;
        } else {
            msg_Warn(vd, "Invalid display state %"PRIx32, display_state.state);
            ret = -1;
        }
    } else {
        msg_Warn(vd, "Failed to query display resolution");
        ret = -1;
    }
    return ret;
}
