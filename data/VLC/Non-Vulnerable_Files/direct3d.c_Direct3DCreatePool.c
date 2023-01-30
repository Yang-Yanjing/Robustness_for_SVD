 */
static int Direct3DCreatePool(vout_display_t *vd, video_format_t *fmt)
{
    vout_display_sys_t *sys = vd->sys;
    LPDIRECT3DDEVICE9 d3ddev = sys->d3ddev;
    /* */
    *fmt = vd->source;
    /* Find the appropriate D3DFORMAT for the render chroma, the format will be the closest to
     * the requested chroma which is usable by the hardware in an offscreen surface, as they
     * typically support more formats than textures */
    const d3d_format_t *d3dfmt = Direct3DFindFormat(vd, fmt->i_chroma, sys->d3dpp.BackBufferFormat);
    if (!d3dfmt) {
        msg_Err(vd, "surface pixel format is not supported.");
        return VLC_EGENERIC;
    }
    fmt->i_chroma = d3dfmt->fourcc;
    fmt->i_rmask  = d3dfmt->rmask;
    fmt->i_gmask  = d3dfmt->gmask;
    fmt->i_bmask  = d3dfmt->bmask;
    /* We create one picture.
     * It is useless to create more as we can't be used for direct rendering */
    /* Create a surface */
    LPDIRECT3DSURFACE9 surface;
    HRESULT hr = IDirect3DDevice9_CreateOffscreenPlainSurface(d3ddev,
                                                              fmt->i_width,
                                                              fmt->i_height,
                                                              d3dfmt->format,
                                                              D3DPOOL_DEFAULT,
                                                              &surface,
                                                              NULL);
    if (FAILED(hr)) {
        msg_Err(vd, "Failed to create picture surface. (hr=0x%lx)", hr);
        return VLC_EGENERIC;
    }
    /* fill surface with black color */
    IDirect3DDevice9_ColorFill(d3ddev, surface, NULL, D3DCOLOR_ARGB(0xFF, 0, 0, 0));
    /* Create the associated picture */
    picture_sys_t *picsys = malloc(sizeof(*picsys));
    if (unlikely(picsys == NULL)) {
        IDirect3DSurface9_Release(surface);
        return VLC_ENOMEM;
    }
    picsys->surface = surface;
    picsys->fallback = NULL;
    picture_resource_t resource = { .p_sys = picsys };
    for (int i = 0; i < PICTURE_PLANE_MAX; i++)
        resource.p[i].i_lines = fmt->i_height / (i > 0 ? 2 : 1);
    picture_t *picture = picture_NewFromResource(fmt, &resource);
    if (!picture) {
        IDirect3DSurface9_Release(surface);
        free(picsys);
        return VLC_ENOMEM;
    }
    sys->picsys = picsys;
    /* Wrap it into a picture pool */
    picture_pool_configuration_t pool_cfg;
    memset(&pool_cfg, 0, sizeof(pool_cfg));
    pool_cfg.picture_count = 1;
    pool_cfg.picture       = &picture;
    pool_cfg.lock          = Direct3DLockSurface;
    pool_cfg.unlock        = Direct3DUnlockSurface;
    sys->pool = picture_pool_NewExtended(&pool_cfg);
    if (!sys->pool) {
        picture_Release(picture);
        IDirect3DSurface9_Release(surface);
        return VLC_ENOMEM;
    }
    return VLC_SUCCESS;
}
