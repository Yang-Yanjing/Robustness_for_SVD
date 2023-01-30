 */
static int DirectXLockSurface(LPDIRECTDRAWSURFACE2 front_surface,
                              LPDIRECTDRAWSURFACE2 surface,
                              DDSURFACEDESC *ddsd)
{
    HRESULT hr;
    DDSURFACEDESC ddsd_dummy;
    if (!ddsd)
        ddsd = &ddsd_dummy;
    ZeroMemory(ddsd, sizeof(*ddsd));
    ddsd->dwSize = sizeof(*ddsd);
    hr = IDirectDrawSurface2_Lock(surface, NULL, ddsd, DDLOCK_NOSYSLOCK | DDLOCK_WAIT, NULL);
    if (hr != DD_OK) {
        if (hr == DDERR_INVALIDPARAMS) {
            /* DirectX 3 doesn't support the DDLOCK_NOSYSLOCK flag, resulting
             * in an invalid params error */
            hr = IDirectDrawSurface2_Lock(surface, NULL, ddsd, DDLOCK_WAIT, NULL);
        }
        if (hr == DDERR_SURFACELOST) {
            /* Your surface can be lost so be sure
             * to check this and restore it if needed */
            /* When using overlays with back-buffers, we need to restore
             * the front buffer so the back-buffers get restored as well. */
            if (front_surface != surface)
                IDirectDrawSurface2_Restore(front_surface);
            else
                IDirectDrawSurface2_Restore(surface);
            hr = IDirectDrawSurface2_Lock(surface, NULL, ddsd, DDLOCK_WAIT, NULL);
        }
        if (hr != DD_OK)
            return VLC_EGENERIC;
    }
    return VLC_SUCCESS;
}
