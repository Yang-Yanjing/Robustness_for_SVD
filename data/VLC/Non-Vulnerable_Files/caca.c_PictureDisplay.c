 */
static void PictureDisplay(vout_display_t *vd, picture_t *picture, subpicture_t *subpicture)
{
    Refresh(vd);
    picture_Release(picture);
    VLC_UNUSED(subpicture);
}
