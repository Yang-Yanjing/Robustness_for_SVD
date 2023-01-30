#define POOL_SIZE 31
static int Open(vlc_object_t *p_this)
{
    vout_display_t *vd = (vout_display_t*)p_this;
    video_format_t fmt = vd->fmt;
    if (fmt.i_chroma != VLC_CODEC_ANDROID_OPAQUE)
        return VLC_EGENERIC;
    /* Allocate structure */
    vout_display_sys_t *sys = (struct vout_display_sys_t*)calloc(1, sizeof(*sys));
    if (!sys)
        return VLC_ENOMEM;
    sys->p_library = LoadNativeWindowAPI(&sys->native_window);
    if (!sys->p_library)
    {
        free(sys);
        msg_Err(vd, "Could not initialize NativeWindow API.");
        return VLC_EGENERIC;
    }
    sys->fmt = fmt;
    video_format_t subpicture_format = sys->fmt;
    subpicture_format.i_chroma = VLC_CODEC_RGBA;
    /* Create a RGBA picture for rendering subtitles. */
    sys->subtitles_picture = picture_NewFromFormat(&subpicture_format);
    /* Export the subpicture capability of this vout. */
    vd->info.subpicture_chromas = subpicture_chromas;
    int i_pictures = POOL_SIZE;
    picture_t** pictures = calloc(sizeof(*pictures), i_pictures);
    if (!pictures)
        goto error;
    for (int i = 0; i < i_pictures; i++)
    {
        picture_sys_t *p_picsys = calloc(1, sizeof(*p_picsys));
        if (unlikely(p_picsys == NULL))
            goto error;
        picture_resource_t resource = { .p_sys = p_picsys };
        picture_t *picture = picture_NewFromResource(&fmt, &resource);
        if (!picture)
        {
            free(p_picsys);
            goto error;
        }
        pictures[i] = picture;
    }
    /* Wrap it into a picture pool */
    picture_pool_configuration_t pool_cfg;
    memset(&pool_cfg, 0, sizeof(pool_cfg));
    pool_cfg.picture_count = i_pictures;
    pool_cfg.picture       = pictures;
    pool_cfg.lock          = LockSurface;
    pool_cfg.unlock        = UnlockSurface;
    sys->pool = picture_pool_NewExtended(&pool_cfg);
    if (!sys->pool)
    {
        for (int i = 0; i < i_pictures; i++)
            picture_Release(pictures[i]);
        goto error;
    }
    /* Setup vout_display */
    vd->sys     = sys;
    vd->fmt     = fmt;
    vd->pool    = Pool;
    vd->display = Display;
    vd->control = Control;
    vd->prepare = NULL;
    vd->manage  = Manage;
    /* Fix initial state */
    vout_display_SendEventFullscreen(vd, false);
    return VLC_SUCCESS;
error:
    free(pictures);
    Close(p_this);
    return VLC_ENOMEM;
}
