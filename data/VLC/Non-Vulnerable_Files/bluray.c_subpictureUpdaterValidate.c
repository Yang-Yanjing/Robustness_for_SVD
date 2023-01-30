 *****************************************************************************/
static int subpictureUpdaterValidate(subpicture_t *p_subpic,
                                      bool b_fmt_src, const video_format_t *p_fmt_src,
                                      bool b_fmt_dst, const video_format_t *p_fmt_dst,
                                      mtime_t i_ts)
{
    VLC_UNUSED(b_fmt_src);
    VLC_UNUSED(b_fmt_dst);
    VLC_UNUSED(p_fmt_src);
    VLC_UNUSED(p_fmt_dst);
    VLC_UNUSED(i_ts);
    subpicture_updater_sys_t *p_upd_sys = p_subpic->updater.p_sys;
    bluray_overlay_t         *p_overlay = p_upd_sys->p_overlay;
    vlc_mutex_lock(&p_overlay->lock);
    int res = p_overlay->status == Outdated;
    vlc_mutex_unlock(&p_overlay->lock);
    return res;
}
