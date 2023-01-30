 */
static int Direct3DLockSurface(picture_t *picture)
{
    /* Lock the surface to get a valid pointer to the picture buffer */
    D3DLOCKED_RECT d3drect;
    HRESULT hr = IDirect3DSurface9_LockRect(picture->p_sys->surface, &d3drect, NULL, 0);
    if (FAILED(hr)) {
        //msg_Dbg(vd, "%s:%d (hr=0x%0lX)", __FUNCTION__, __LINE__, hr);
        return CommonUpdatePicture(picture, &picture->p_sys->fallback, NULL, 0);
    }
    CommonUpdatePicture(picture, NULL, d3drect.pBits, d3drect.Pitch);
    return VLC_SUCCESS;
}
