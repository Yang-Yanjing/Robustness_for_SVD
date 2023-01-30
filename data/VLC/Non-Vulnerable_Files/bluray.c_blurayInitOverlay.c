}
static void blurayInitOverlay(demux_t *p_demux, int plane, int width, int height)
{
    demux_sys_t *p_sys = p_demux->p_sys;
    assert(p_sys->p_overlays[plane] == NULL);
    bluray_overlay_t *ov = calloc(1, sizeof(*ov));
    if (unlikely(ov == NULL))
        return;
    subpicture_updater_sys_t *p_upd_sys = malloc(sizeof(*p_upd_sys));
    if (unlikely(p_upd_sys == NULL)) {
        free(ov);
        return;
    }
    /* two references: vout + demux */
    atomic_flag_clear(&ov->released_once);
    p_upd_sys->p_overlay = ov;
    subpicture_updater_t updater = {
        .pf_validate = subpictureUpdaterValidate,
        .pf_update   = subpictureUpdaterUpdate,
        .pf_destroy  = subpictureUpdaterDestroy,
        .p_sys       = p_upd_sys,
    };
    ov->p_pic = subpicture_New(&updater);
    if (ov->p_pic == NULL) {
        free(p_upd_sys);
        free(ov);
        return;
    }
    ov->p_pic->i_original_picture_width = width;
    ov->p_pic->i_original_picture_height = height;
    ov->p_pic->b_ephemer = true;
    ov->p_pic->b_absolute = true;
    vlc_mutex_init(&ov->lock);
    p_sys->p_overlays[plane] = ov;
}
