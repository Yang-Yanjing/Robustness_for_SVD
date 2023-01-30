}
static void subpictureUpdaterUpdate(subpicture_t *p_subpic,
                                    const video_format_t *p_fmt_src,
                                    const video_format_t *p_fmt_dst,
                                    mtime_t i_ts)
{
    VLC_UNUSED(p_fmt_src);
    VLC_UNUSED(p_fmt_dst);
    VLC_UNUSED(i_ts);
    subpicture_updater_sys_t *p_upd_sys = p_subpic->updater.p_sys;
    bluray_overlay_t         *p_overlay = p_upd_sys->p_overlay;
    /*
     * When this function is called, all p_subpic regions are gone.
     * We need to duplicate our regions (stored internaly) to this subpic.
     */
    vlc_mutex_lock(&p_overlay->lock);
    subpicture_region_t *p_src = p_overlay->p_regions;
    if (!p_src) {
        vlc_mutex_unlock(&p_overlay->lock);
        return;
    }
    subpicture_region_t **p_dst = &p_subpic->p_region;
    while (p_src != NULL) {
        *p_dst = subpicture_region_Clone(p_src);
        if (*p_dst == NULL)
            break;
        p_dst = &(*p_dst)->p_next;
        p_src = p_src->p_next;
    }
    if (*p_dst != NULL)
        (*p_dst)->p_next = NULL;
    p_overlay->status = Displayed;
    vlc_mutex_unlock(&p_overlay->lock);
}
