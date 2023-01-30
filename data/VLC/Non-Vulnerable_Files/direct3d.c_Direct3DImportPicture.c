 */
static int Direct3DImportPicture(vout_display_t *vd,
                                 d3d_region_t *region,
                                 LPDIRECT3DSURFACE9 source)
{
    vout_display_sys_t *sys = vd->sys;
    HRESULT hr;
    if (!source) {
        msg_Dbg(vd, "no surface to render ?");
        return VLC_EGENERIC;
    }
    /* retrieve texture top-level surface */
    LPDIRECT3DSURFACE9 destination;
    hr = IDirect3DTexture9_GetSurfaceLevel(sys->d3dtex, 0, &destination);
    if (FAILED(hr)) {
        msg_Dbg(vd, "%s:%d (hr=0x%0lX)", __FUNCTION__, __LINE__, hr);
        return VLC_EGENERIC;
    }
    /* Copy picture surface into texture surface
     * color space conversion happen here */
    RECT copy_rect = vd->sys->rect_src_clipped;
    // On nVidia & AMD, StretchRect will fail if the visible size isn't even.
    // When copying the entire buffer, the margin end up being blended in the actual picture
    // on nVidia (regardless of even/odd dimensions)
    if ( copy_rect.right & 1 ) copy_rect.right++;
    if ( copy_rect.bottom & 1 ) copy_rect.bottom++;
    hr = IDirect3DDevice9_StretchRect(sys->d3ddev, source, &copy_rect, destination,
                                      &copy_rect, D3DTEXF_NONE);
    IDirect3DSurface9_Release(destination);
    if (FAILED(hr)) {
        msg_Dbg(vd, "%s:%d (hr=0x%0lX)", __FUNCTION__, __LINE__, hr);
        return VLC_EGENERIC;
    }
    /* */
    region->texture = sys->d3dtex;
    Direct3DSetupVertices(region->vertex, &vd->sys->rect_src, &vd->sys->rect_src_clipped,
                          vd->sys->rect_dest_clipped, 255, vd->fmt.orientation);
    return VLC_SUCCESS;
}
