 */
static void blurayClearOverlay(demux_t *p_demux, int plane)
{
    demux_sys_t *p_sys = p_demux->p_sys;
    bluray_overlay_t *ov = p_sys->p_overlays[plane];
    vlc_mutex_lock(&ov->lock);
    subpicture_region_ChainDelete(ov->p_regions);
    ov->p_regions = NULL;
    ov->status = Outdated;
    vlc_mutex_unlock(&ov->lock);
}
