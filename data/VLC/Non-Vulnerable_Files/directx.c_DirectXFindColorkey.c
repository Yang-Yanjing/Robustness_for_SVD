 */
static uint32_t DirectXFindColorkey(vout_display_t *vd, uint32_t *color)
{
    vout_display_sys_t *sys = vd->sys;
    HRESULT hr;
    /* */
    DDSURFACEDESC ddsd;
    ddsd.dwSize = sizeof(ddsd);
    hr = IDirectDrawSurface2_Lock(sys->display, NULL, &ddsd, DDLOCK_WAIT, NULL);
    if (hr != DD_OK)
        return 0;
    uint32_t backup = *(uint32_t *)ddsd.lpSurface;
    switch (ddsd.ddpfPixelFormat.dwRGBBitCount) {
    case 4:
        *(uint8_t *)ddsd.lpSurface = *color | (*color << 4);
        break;
    case 8:
        *(uint8_t *)ddsd.lpSurface = *color;
        break;
    case 15:
    case 16:
        *(uint16_t *)ddsd.lpSurface = *color;
        break;
    case 24:
        /* Seems to be problematic so we'll just put black as the colorkey */
        *color = 0;
    default:
        *(uint32_t *)ddsd.lpSurface = *color;
        break;
    }
    IDirectDrawSurface2_Unlock(sys->display, NULL);
    /* */
    HDC hdc;
    COLORREF rgb;
    if (IDirectDrawSurface2_GetDC(sys->display, &hdc) == DD_OK) {
        rgb = GetPixel(hdc, 0, 0);
        IDirectDrawSurface2_ReleaseDC(sys->display, hdc);
    } else {
        rgb = 0;
    }
    /* Restore the pixel value */
    ddsd.dwSize = sizeof(ddsd);
    if (IDirectDrawSurface2_Lock(sys->display, NULL, &ddsd, DDLOCK_WAIT, NULL) == DD_OK) {
        *(uint32_t *)ddsd.lpSurface = backup;
        IDirectDrawSurface2_Unlock(sys->display, NULL);
    }
    return rgb;
}
