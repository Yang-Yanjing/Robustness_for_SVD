 */
static void Direct3DUnlockSurface(picture_t *picture)
{
    /* Unlock the Surface */
    HRESULT hr = IDirect3DSurface9_UnlockRect(picture->p_sys->surface);
    if (FAILED(hr)) {
        //msg_Dbg(vd, "%s:%d (hr=0x%0lX)", __FUNCTION__, __LINE__, hr);
    }
}
