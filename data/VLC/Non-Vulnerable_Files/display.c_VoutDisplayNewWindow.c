}
static vout_window_t *VoutDisplayNewWindow(vout_display_t *vd, const vout_window_cfg_t *cfg)
{
    vout_display_owner_sys_t *osys = vd->owner.sys;
#ifdef ALLOW_DUMMY_VOUT
    if (!osys->vout->p) {
        vout_window_cfg_t cfg_override = *cfg;
        if (!var_InheritBool(osys->vout, "embedded-video"))
            cfg_override.is_standalone = true;
        return vout_window_New(VLC_OBJECT(osys->vout), "$window", &cfg_override);
    }
#endif
    return vout_NewDisplayWindow(osys->vout, vd, cfg);
}
