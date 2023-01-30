}
static void blurayOverlayProc(void *ptr, const BD_OVERLAY *const overlay)
{
    demux_t *p_demux = (demux_t*)ptr;
    demux_sys_t *p_sys = p_demux->p_sys;
    if (!overlay) {
        msg_Info(p_demux, "Closing overlays.");
        p_sys->current_overlay = -1;
        if (p_sys->p_vout)
            for (int i = 0; i < MAX_OVERLAY; i++)
                blurayCloseOverlay(p_demux, i);
        return;
    }
    switch (overlay->cmd) {
    case BD_OVERLAY_INIT:
        msg_Info(p_demux, "Initializing overlay");
        blurayInitOverlay(p_demux, overlay->plane, overlay->w, overlay->h);
        break;
    case BD_OVERLAY_CLOSE:
        blurayClearOverlay(p_demux, overlay->plane);
        blurayCloseOverlay(p_demux, overlay->plane);
        break;
    case BD_OVERLAY_CLEAR:
        blurayClearOverlay(p_demux, overlay->plane);
        break;
    case BD_OVERLAY_FLUSH:
        blurayActivateOverlay(p_demux, overlay->plane);
        break;
    case BD_OVERLAY_DRAW:
        blurayDrawOverlay(p_demux, overlay);
        break;
    default:
        msg_Warn(p_demux, "Unknown BD overlay command: %u", overlay->cmd);
        break;
    }
}
