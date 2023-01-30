 */
static int DirectXUpdateOverlay(vout_display_t *vd, LPDIRECTDRAWSURFACE2 surface)
{
    vout_display_sys_t *sys = vd->sys;
    RECT src = sys->rect_src_clipped;
    RECT dst = sys->rect_dest_clipped;
    if (sys->use_wallpaper) {
        src.left   = vd->source.i_x_offset;
        src.top    = vd->source.i_y_offset;
        src.right  = vd->source.i_x_offset + vd->source.i_visible_width;
        src.bottom = vd->source.i_y_offset + vd->source.i_visible_height;
        AlignRect(&src, sys->i_align_src_boundary, sys->i_align_src_size);
        vout_display_cfg_t cfg = *vd->cfg;
        cfg.display.width  = sys->rect_display.right;
        cfg.display.height = sys->rect_display.bottom;
        vout_display_place_t place;
        vout_display_PlacePicture(&place, &vd->source, &cfg, true);
        dst.left   = sys->rect_display.left + place.x;
        dst.top    = sys->rect_display.top  + place.y;
        dst.right  = dst.left + place.width;
        dst.bottom = dst.top  + place.height;
        AlignRect(&dst, sys->i_align_dest_boundary, sys->i_align_dest_size);
    }
    if (!surface) {
        if (!sys->pool)
            return VLC_EGENERIC;
        surface = sys->picsys->front_surface;
    }
    /* The new window dimensions should already have been computed by the
     * caller of this function */
    /* Position and show the overlay */
    DDOVERLAYFX ddofx;
    ZeroMemory(&ddofx, sizeof(ddofx));
    ddofx.dwSize = sizeof(ddofx);
    ddofx.dckDestColorkey.dwColorSpaceLowValue = sys->i_colorkey;
    ddofx.dckDestColorkey.dwColorSpaceHighValue = sys->i_colorkey;
    HRESULT hr = IDirectDrawSurface2_UpdateOverlay(surface,
                                                   &src, sys->display, &dst,
                                                   DDOVER_SHOW | DDOVER_KEYDESTOVERRIDE, &ddofx);
    sys->restore_overlay = hr != DD_OK;
    if (hr != DD_OK) {
        msg_Warn(vd, "DirectDrawUpdateOverlay cannot move/resize overlay. (hr=0x%lX)", hr);
        return VLC_EGENERIC;
    }
    return VLC_SUCCESS;
}
