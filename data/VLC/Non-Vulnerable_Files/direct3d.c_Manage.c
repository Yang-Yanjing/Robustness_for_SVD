}
static void Manage (vout_display_t *vd)
{
    vout_display_sys_t *sys = vd->sys;
    CommonManage(vd);
    /* Desktop mode change */
    vlc_mutex_lock(&sys->lock);
    const bool ch_desktop = sys->ch_desktop;
    sys->ch_desktop = false;
    vlc_mutex_unlock(&sys->lock);
    if (ch_desktop) {
        sys->reopen_device = true;
        vout_display_SendEventPicturesInvalid(vd);
    }
    /* Position Change */
    if (sys->changes & DX_POSITION_CHANGE) {
#if 0 /* need that when bicubic filter is available */
        RECT rect;
        UINT width, height;
        GetClientRect(p_sys->hvideownd, &rect);
        width  = rect.right-rect.left;
        height = rect.bottom-rect.top;
        if (width != p_sys->d3dpp.BackBufferWidth || height != p_sys->d3dpp.BackBufferHeight)
        {
            msg_Dbg(vd, "resizing device back buffers to (%lux%lu)", width, height);
            // need to reset D3D device to resize back buffer
            if (VLC_SUCCESS != Direct3DResetDevice(vd, width, height))
                return VLC_EGENERIC;
        }
#endif
        sys->clear_scene = true;
        sys->changes &= ~DX_POSITION_CHANGE;
    }
}
