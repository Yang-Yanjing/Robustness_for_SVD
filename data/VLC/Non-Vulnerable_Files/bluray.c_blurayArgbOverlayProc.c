}
static void blurayArgbOverlayProc(void *ptr, const BD_ARGB_OVERLAY *const overlay)
{
    demux_t *p_demux = (demux_t*)ptr;
    switch (overlay->cmd) {
    case BD_ARGB_OVERLAY_INIT:
        blurayInitArgbOverlay(p_demux, overlay->plane, overlay->w, overlay->h);
        break;
    case BD_ARGB_OVERLAY_CLOSE:
        blurayClearOverlay(p_demux, overlay->plane);
        blurayCloseOverlay(p_demux, overlay->plane);
        break;
    case BD_ARGB_OVERLAY_FLUSH:
        blurayActivateOverlay(p_demux, overlay->plane);
        break;
    case BD_ARGB_OVERLAY_DRAW:
        blurayDrawArgbOverlay(p_demux, overlay);
        break;
    default:
        msg_Warn(p_demux, "Unknown BD ARGB overlay command: %u", overlay->cmd);
        break;
    }
}
