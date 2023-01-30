 */
static void vout_display_Delete(vout_display_t *vd)
{
    if (vd->module)
        module_unneed(vd, vd->module);
    video_format_Clean(&vd->source);
    video_format_Clean(&vd->fmt);
    vlc_object_release(vd);
}
