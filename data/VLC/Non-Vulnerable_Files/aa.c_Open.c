 */
static int Open(vlc_object_t *object)
{
    vout_display_t *vd = (vout_display_t *)object;
    vout_display_sys_t *sys;
#ifndef _WIN32
    if (!vlc_xlib_init (object))
        return VLC_EGENERIC;
#endif
    /* Allocate structure */
    vd->sys = sys = calloc(1, sizeof(*sys));
    if (!sys)
        return VLC_ENOMEM;
    /* Don't parse any options, but take $AAOPTS into account */
    aa_parseoptions(NULL, NULL, NULL, NULL);
    /* */
    sys->aa_context = aa_autoinit(&aa_defparams);
    if (!sys->aa_context) {
        msg_Err(vd, "cannot initialize aalib");
        goto error;
    }
    vout_display_DeleteWindow(vd, NULL);
    aa_autoinitkbd(sys->aa_context, 0);
    aa_autoinitmouse(sys->aa_context, AA_MOUSEALLMASK);
    /* */
    video_format_t fmt = vd->fmt;
    fmt.i_chroma = VLC_CODEC_RGB8;
    fmt.i_width  = aa_imgwidth(sys->aa_context);
    fmt.i_height = aa_imgheight(sys->aa_context);
    fmt.i_visible_width = fmt.i_width;
    fmt.i_visible_height = fmt.i_height;
    /* */
    vout_display_info_t info = vd->info;
    info.has_pictures_invalid = true;
    /* Setup vout_display now that everything is fine */
    vd->fmt = fmt;
    vd->info = info;
    vd->pool    = Pool;
    vd->prepare = Prepare;
    vd->display = PictureDisplay;
    vd->control = Control;
    vd->manage  = Manage;
    /* Inspect initial configuration and send correction events
     * FIXME how to handle aspect ratio with aa ? */
    sys->state = *vd->cfg;
    sys->state.is_fullscreen = false;
    vout_display_SendEventFullscreen(vd, false);
    vout_display_SendEventDisplaySize(vd, fmt.i_width, fmt.i_height, false);
    return VLC_SUCCESS;
error:
    if (sys && sys->aa_context)
        aa_close(sys->aa_context);
    free(sys);
    return VLC_EGENERIC;
}
