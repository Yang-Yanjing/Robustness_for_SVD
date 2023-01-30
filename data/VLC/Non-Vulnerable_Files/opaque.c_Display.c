}
static void Display(vout_display_t *vd, picture_t *picture, subpicture_t *subpicture)
{
    VLC_UNUSED(vd);
    VLC_UNUSED(subpicture);
    picture_sys_t *p_picsys = picture->p_sys;
    vout_display_sys_t *sys = vd->sys;
    void (*display_callback)(picture_sys_t*) = p_picsys->pf_display_callback;
    if (display_callback)
        display_callback(p_picsys);
    if (subpicture)
	sys->b_has_subpictures = true;
    /* As long as no subpicture was received, do not call
       DisplaySubpicture since JNI calls and clearing the subtitles
       surface are expensive operations. */
    if (sys->b_has_subpictures)
    {
	DisplaySubpicture(vd, subpicture);
        if (!subpicture)
        {
            /* The surface has been cleared and there is no new
               subpicture to upload, do not clear again until a new
               subpicture is received. */
            sys->b_has_subpictures = false;
        }
    }
    /* refcount lowers to 0, and pool_cfg.unlock is called */
    picture_Release(picture);
    if (subpicture)
        subpicture_Delete(subpicture);
}
