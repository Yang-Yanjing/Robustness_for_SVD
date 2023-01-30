};
static vout_window_t *SplitterNewWindow(vout_display_t *vd, const vout_window_cfg_t *cfg_ptr)
{
    vout_display_owner_sys_t *osys = vd->owner.sys;
    vout_window_cfg_t cfg = *cfg_ptr;
    cfg.is_standalone = true;
    cfg.x += 0;//output->window.i_x; FIXME
    cfg.y += 0;//output->window.i_y;
    return vout_NewDisplayWindow(osys->vout, vd, &cfg);
}
