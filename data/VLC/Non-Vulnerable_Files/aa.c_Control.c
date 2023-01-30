 */
static int Control(vout_display_t *vd, int query, va_list args)
{
    VLC_UNUSED(args);
    vout_display_sys_t *sys = vd->sys;
    switch (query) {
    case VOUT_DISPLAY_CHANGE_DISPLAY_SIZE:
        /* We have to ignore what is requested */
        vout_display_SendEventPicturesInvalid(vd);
        return VLC_SUCCESS;
    case VOUT_DISPLAY_RESET_PICTURES:
        if (sys->pool)
            picture_pool_Delete(sys->pool);
        sys->pool = NULL;
        vd->fmt.i_width  = aa_imgwidth(sys->aa_context);
        vd->fmt.i_height = aa_imgheight(sys->aa_context);
        return VLC_SUCCESS;
    case VOUT_DISPLAY_HIDE_MOUSE:
        aa_hidemouse(sys->aa_context);
        return VLC_SUCCESS;
    default:
        msg_Err(vd, "Unsupported query in vout display aalib");
        return VLC_EGENERIC;
    }
}
