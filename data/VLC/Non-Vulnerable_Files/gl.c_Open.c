 */
static int Open (vlc_object_t *obj)
{
    vout_display_t *vd = (vout_display_t *)obj;
    vout_display_sys_t *sys = malloc (sizeof (*sys));
    if (unlikely(sys == NULL))
        return VLC_ENOMEM;
    sys->gl = NULL;
    sys->pool = NULL;
    sys->window = MakeWindow (vd);
    if (sys->window == NULL)
        goto error;
    sys->gl = vlc_gl_Create (sys->window, API, "$" MODULE_VARNAME);
    if (sys->gl == NULL)
        goto error;
    /* Initialize video display */
    const vlc_fourcc_t *spu_chromas;
    if (vlc_gl_MakeCurrent (sys->gl))
        goto error;
    sys->vgl = vout_display_opengl_New (&vd->fmt, &spu_chromas, sys->gl);
    vlc_gl_ReleaseCurrent (sys->gl);
    if (sys->vgl == NULL)
        goto error;
    vd->sys = sys;
    vd->info.has_pictures_invalid = false;
    vd->info.has_event_thread = false;
    vd->info.subpicture_chromas = spu_chromas;
    vd->pool = Pool;
    vd->prepare = PictureRender;
    vd->display = PictureDisplay;
    vd->control = Control;
    vd->manage = NULL;
    return VLC_SUCCESS;
error:
    if (sys->gl != NULL)
        vlc_gl_Destroy (sys->gl);
    if (sys->window != NULL)
        vout_display_DeleteWindow (vd, sys->window);
    free (sys);
    return VLC_EGENERIC;
}
