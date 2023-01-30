 */
static int Open (vlc_object_t *obj)
{
    vout_display_t *vd = (vout_display_t *)obj;
    vout_display_sys_t *sys = malloc (sizeof (*sys));
    if (sys == NULL)
        return VLC_ENOMEM;
    sys->vgl = NULL;
    sys->pool = NULL;
    /* Get window, connect to X server (via XCB) */
    xcb_connection_t *conn;
    const xcb_screen_t *scr;
    uint16_t width, height;
    sys->embed = XCB_parent_Create (vd, &conn, &scr, &width, &height);
    if (sys->embed == NULL)
    {
        free (sys);
        return VLC_EGENERIC;
    }
    sys->conn = conn;
    sys->gl = vlc_gl_Create (sys->embed, VLC_OPENGL, "glx");
    if (sys->gl == NULL)
        goto error;
    const vlc_fourcc_t *spu_chromas;
    if (vlc_gl_MakeCurrent (sys->gl))
        goto error;
    sys->vgl = vout_display_opengl_New (&vd->fmt, &spu_chromas, sys->gl);
    vlc_gl_ReleaseCurrent (sys->gl);
    if (sys->vgl == NULL)
        goto error;
    sys->cursor = XCB_cursor_Create (conn, scr);
    sys->visible = false;
    /* Setup vout_display_t once everything is fine */
    vd->sys = sys;
    vd->info.has_pictures_invalid = false;
    vd->info.has_event_thread = true;
    vd->info.subpicture_chromas = spu_chromas;
    vd->pool = Pool;
    vd->prepare = PictureRender;
    vd->display = PictureDisplay;
    vd->control = Control;
    vd->manage = Manage;
    /* */
    bool is_fullscreen = vd->cfg->is_fullscreen;
    if (is_fullscreen && vout_window_SetFullScreen (sys->embed, true))
        is_fullscreen = false;
    vout_display_SendEventFullscreen (vd, is_fullscreen);
    vout_display_SendEventDisplaySize (vd, width, height, is_fullscreen);
    return VLC_SUCCESS;
error:
    if (sys->gl != NULL)
        vlc_gl_Destroy (sys->gl);
    xcb_disconnect (sys->conn);
    vout_display_DeleteWindow (vd, sys->embed);
    free (sys);
    return VLC_EGENERIC;
}
