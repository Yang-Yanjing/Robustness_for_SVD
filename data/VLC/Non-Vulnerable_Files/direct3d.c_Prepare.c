static void Direct3DUnlockSurface(picture_t *);
static void Prepare(vout_display_t *vd, picture_t *picture, subpicture_t *subpicture)
{
    vout_display_sys_t *sys = vd->sys;
    LPDIRECT3DSURFACE9 surface = picture->p_sys->surface;
#if 0
    picture_Release(picture);
    VLC_UNUSED(subpicture);
#else
    /* FIXME it is a bit ugly, we need the surface to be unlocked for
     * rendering.
     *  The clean way would be to release the picture (and ensure that
     * the vout doesn't keep a reference). But because of the vout
     * wrapper, we can't */
    Direct3DUnlockSurface(picture);
    VLC_UNUSED(subpicture);
#endif
    /* check if device is still available */
    HRESULT hr = IDirect3DDevice9_TestCooperativeLevel(sys->d3ddev);
    if (FAILED(hr)) {
        if (hr == D3DERR_DEVICENOTRESET && !sys->reset_device) {
            vout_display_SendEventPicturesInvalid(vd);
            sys->reset_device = true;
            sys->lost_not_ready = false;
        }
        if (hr == D3DERR_DEVICELOST && !sys->lost_not_ready) {
            /* Device is lost but not yet ready for reset. */
            sys->lost_not_ready = true;
        }
        return;
    }
    d3d_region_t picture_region;
    if (!Direct3DImportPicture(vd, &picture_region, surface)) {
        picture_region.width = picture->format.i_width;
        picture_region.height = picture->format.i_height;
        int subpicture_region_count     = 0;
        d3d_region_t *subpicture_region = NULL;
        if (subpicture)
            Direct3DImportSubpicture(vd, &subpicture_region_count, &subpicture_region,
                                     subpicture);
        Direct3DRenderScene(vd, &picture_region,
                            subpicture_region_count, subpicture_region);
        Direct3DDeleteRegions(sys->d3dregion_count, sys->d3dregion);
        sys->d3dregion_count = subpicture_region_count;
        sys->d3dregion       = subpicture_region;
    }
}
