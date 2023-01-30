}
static void bluraySendOverlayToVout(demux_t *p_demux)
{
    demux_sys_t *p_sys = p_demux->p_sys;
    assert(p_sys->current_overlay >= 0 &&
           p_sys->p_overlays[p_sys->current_overlay] != NULL &&
           p_sys->p_overlays[p_sys->current_overlay]->p_pic != NULL);
    p_sys->p_overlays[p_sys->current_overlay]->p_pic->i_start =
        p_sys->p_overlays[p_sys->current_overlay]->p_pic->i_stop = mdate();
    p_sys->p_overlays[p_sys->current_overlay]->p_pic->i_channel =
        vout_RegisterSubpictureChannel(p_sys->p_vout);
    /*
     * After this point, the picture should not be accessed from the demux thread,
     * as it is held by the vout thread.
     * This must be done only once per subpicture, ie. only once between each
     * blurayInitOverlay & blurayCloseOverlay call.
     */
    vout_PutSubpicture(p_sys->p_vout, p_sys->p_overlays[p_sys->current_overlay]->p_pic);
    /*
     * Mark the picture as Outdated, as it contains no region for now.
     * This will make the subpicture_updater_t call pf_update
     */
    p_sys->p_overlays[p_sys->current_overlay]->status = Outdated;
}
