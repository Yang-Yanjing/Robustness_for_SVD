}
static int Extract(vlc_va_t *va, picture_t *picture, void *opaque,
                   uint8_t *data)
{
    vlc_va_sys_t *sys = va->sys;
    LPDIRECT3DSURFACE9 d3d = (LPDIRECT3DSURFACE9)(uintptr_t)data;
    if (!sys->surface_cache.buffer)
        return VLC_EGENERIC;
    /* */
    assert(sys->output == MAKEFOURCC('Y','V','1','2'));
    /* */
    D3DLOCKED_RECT lock;
    if (FAILED(IDirect3DSurface9_LockRect(d3d, &lock, NULL, D3DLOCK_READONLY))) {
        msg_Err(va, "Failed to lock surface");
        return VLC_EGENERIC;
    }
    if (sys->render == MAKEFOURCC('Y','V','1','2') ||
        sys->render == MAKEFOURCC('I','M','C','3')) {
        bool imc3 = sys->render == MAKEFOURCC('I','M','C','3');
        size_t chroma_pitch = imc3 ? lock.Pitch : (lock.Pitch / 2);
        size_t pitch[3] = {
            lock.Pitch,
            chroma_pitch,
            chroma_pitch,
        };
        uint8_t *plane[3] = {
            (uint8_t*)lock.pBits,
            (uint8_t*)lock.pBits + pitch[0] * sys->surface_height,
            (uint8_t*)lock.pBits + pitch[0] * sys->surface_height
                                 + pitch[1] * sys->surface_height / 2,
        };
        if (imc3) {
            uint8_t *V = plane[1];
            plane[1] = plane[2];
            plane[2] = V;
        }
        CopyFromYv12(picture, plane, pitch, sys->width, sys->height,
                     &sys->surface_cache);
    } else {
        assert(sys->render == MAKEFOURCC('N','V','1','2'));
        uint8_t *plane[2] = {
            lock.pBits,
            (uint8_t*)lock.pBits + lock.Pitch * sys->surface_height
        };
        size_t  pitch[2] = {
            lock.Pitch,
            lock.Pitch,
        };
        CopyFromNv12(picture, plane, pitch, sys->width, sys->height,
                     &sys->surface_cache);
    }
    /* */
    IDirect3DSurface9_UnlockRect(d3d);
    (void) opaque;
    return VLC_SUCCESS;
}
