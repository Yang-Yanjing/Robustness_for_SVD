}
static void adjust_refresh_rate(vout_display_t *vd)
{
    TV_DISPLAY_STATE_T display_state;
    TV_SUPPORTED_MODE_NEW_T supported_modes[VC_TV_MAX_MODE_IDS];
    int num_modes;
    double frame_rate = (double)vd->fmt.i_frame_rate / vd->fmt.i_frame_rate_base;
    int best_id = -1;
    double best_score, score;
    int i;
    vc_tv_get_display_state(&display_state);
    if(display_state.display.hdmi.mode != HDMI_MODE_OFF) {
        num_modes = vc_tv_hdmi_get_supported_modes_new(display_state.display.hdmi.group,
                        supported_modes, VC_TV_MAX_MODE_IDS, NULL, NULL);
        for(i = 0; i < num_modes; ++i) {
            if(supported_modes[i].width != display_state.display.hdmi.width ||
                            supported_modes[i].height != display_state.display.hdmi.height ||
                            supported_modes[i].scan_mode != display_state.display.hdmi.scan_mode)
                continue;
            score = fmod(supported_modes[i].frame_rate, frame_rate);
            if((best_id < 0) || (score < best_score)) {
                best_id = i;
                best_score = score;
            }
        }
        if((best_id >= 0) && (display_state.display.hdmi.frame_rate != supported_modes[best_id].frame_rate)) {
            msg_Info(vd, "Setting HDMI refresh rate to %"PRIu32,
                            supported_modes[best_id].frame_rate);
            vc_tv_hdmi_power_on_explicit_new(HDMI_MODE_HDMI,
                            supported_modes[best_id].group,
                            supported_modes[best_id].code);
        }
    }
}
