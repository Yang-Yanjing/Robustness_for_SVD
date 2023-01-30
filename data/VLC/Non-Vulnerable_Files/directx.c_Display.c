}
static void Display(vout_display_t *vd, picture_t *picture, subpicture_t *subpicture)
{
    vout_display_sys_t *sys = vd->sys;
    assert(sys->display);
    /* Our surface can be lost so be sure to check this
     * and restore it if need be */
    if (IDirectDrawSurface2_IsLost(sys->display) == DDERR_SURFACELOST) {
        if (IDirectDrawSurface2_Restore(sys->display) == DD_OK) {
            if (sys->use_overlay)
                DirectXUpdateOverlay(vd, NULL);
        }
    }
    if (sys->restore_overlay)
        DirectXUpdateOverlay(vd, NULL);
    /* */
    DirectXUnlock(picture);
    if (sys->use_overlay) {
        /* Flip the overlay buffers if we are using back buffers */
        if (picture->p_sys->surface != picture->p_sys->front_surface) {
            HRESULT hr = IDirectDrawSurface2_Flip(picture->p_sys->front_surface,
                                                  NULL, DDFLIP_WAIT);
            if (hr != DD_OK)
                msg_Warn(vd, "could not flip overlay (error %li)", hr);
        }
    } else {
        /* Blit video surface to display with the NOTEARING option */
        DDBLTFX  ddbltfx;
        ZeroMemory(&ddbltfx, sizeof(ddbltfx));
        ddbltfx.dwSize = sizeof(ddbltfx);
        ddbltfx.dwDDFX = DDBLTFX_NOTEARING;
        HRESULT hr = IDirectDrawSurface2_Blt(sys->display,
                                             &sys->rect_dest_clipped,
                                             picture->p_sys->surface,
                                             &sys->rect_src_clipped,
                                             DDBLT_ASYNC, &ddbltfx);
        if (hr != DD_OK)
            msg_Warn(vd, "could not blit surface (error %li)", hr);
    }
    DirectXLock(picture);
    if (sys->is_first_display) {
        IDirectDraw_WaitForVerticalBlank(sys->ddobject,
                                         DDWAITVB_BLOCKBEGIN, NULL);
        if (sys->use_overlay) {
            HBRUSH brush = CreateSolidBrush(sys->i_rgb_colorkey);
            /* set the colorkey as the backgound brush for the video window */
            SetClassLongPtr(sys->hvideownd, GCLP_HBRBACKGROUND, (LONG_PTR)brush);
        }
    }
    CommonDisplay(vd);
    picture_Release(picture);
    VLC_UNUSED(subpicture);
}
