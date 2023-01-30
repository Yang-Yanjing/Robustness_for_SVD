}
static void blurayCloseOverlay(demux_t *p_demux, int plane)
{
    demux_sys_t *p_sys = p_demux->p_sys;
    bluray_overlay_t *ov = p_sys->p_overlays[plane];
    if (ov != NULL) {
        if (p_sys->p_vout)
            vout_FlushSubpictureChannel(p_sys->p_vout, ov->p_pic->i_channel);
        blurayCleanOverlayStruct(ov);
        if (p_sys->current_overlay == plane)
            p_sys->current_overlay = -1;
        p_sys->p_overlays[plane] = NULL;
    }
    for (int i = 0; i < MAX_OVERLAY; i++)
        if (p_sys->p_overlays[i])
            return;
    /* All overlays have been closed */
    if (p_sys->p_vout != NULL) {
        var_DelCallback(p_sys->p_vout, "mouse-moved", onMouseEvent, p_demux);
        var_DelCallback(p_sys->p_vout, "mouse-clicked", onMouseEvent, p_demux);
        vlc_object_release(p_sys->p_vout);
        p_sys->p_vout = NULL;
    }
}
