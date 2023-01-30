 */
static void PictureDisplay(vout_display_t *vd, picture_t *picture, subpicture_t *subpicture)
{
    vout_display_sys_t *sys = vd->sys;
    aa_flush(sys->aa_context);
    picture_Release(picture);
    VLC_UNUSED(subpicture);
}
