 */
static vout_display_t *vout_display_New(vlc_object_t *obj,
                                        const char *module, bool load_module,
                                        const video_format_t *fmt,
                                        const vout_display_cfg_t *cfg,
                                        vout_display_owner_t *owner)
{
    /* */
    vout_display_t *vd = vlc_custom_create(obj, sizeof(*vd), "vout display" );
    /* */
    video_format_Copy(&vd->source, fmt);
    /* Picture buffer does not have the concept of aspect ratio */
    video_format_Copy(&vd->fmt, fmt);
    vd->fmt.i_sar_num = 0;
    vd->fmt.i_sar_den = 0;
    vd->info.is_slow = false;
    vd->info.has_double_click = false;
    vd->info.has_hide_mouse = false;
    vd->info.has_pictures_invalid = false;
    vd->info.has_event_thread = false;
    vd->info.subpicture_chromas = NULL;
    vd->cfg = cfg;
    vd->pool = NULL;
    vd->prepare = NULL;
    vd->display = NULL;
    vd->control = NULL;
    vd->manage = NULL;
    vd->sys = NULL;
    vd->owner = *owner;
    if (load_module) {
        vd->module = module_need(vd, "vout display", module, module && *module != '\0');
        if (!vd->module) {
            vlc_object_release(vd);
            return NULL;
        }
    } else {
        vd->module = NULL;
    }
    return vd;
}
