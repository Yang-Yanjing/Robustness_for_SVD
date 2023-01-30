}
static int DirectXCreatePool(vout_display_t *vd,
                             bool *use_overlay, video_format_t *fmt)
{
    vout_display_sys_t *sys = vd->sys;
    /* */
    *fmt = vd->source;
    if (DirectXCreatePictureResource(vd, use_overlay, fmt))
        return VLC_EGENERIC;
    /* Create the associated picture */
    picture_resource_t resource = { .p_sys = sys->picsys };
    picture_t *picture = picture_NewFromResource(fmt, &resource);
    if (!picture) {
        DirectXDestroyPictureResource(vd);
        free(sys->picsys);
        return VLC_ENOMEM;
    }
    /* Wrap it into a picture pool */
    picture_pool_configuration_t cfg;
    memset(&cfg, 0, sizeof(cfg));
    cfg.picture_count = 1;
    cfg.picture       = &picture;
    cfg.lock          = DirectXLock;
    cfg.unlock        = DirectXUnlock;
    sys->pool = picture_pool_NewExtended(&cfg);
    if (!sys->pool) {
        picture_Release(picture);
        DirectXDestroyPictureResource(vd);
        return VLC_ENOMEM;
    }
    return VLC_SUCCESS;
}
