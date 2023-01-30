 */
static void Direct3DDestroyPool(vout_display_t *vd)
{
    vout_display_sys_t *sys = vd->sys;
    if (sys->pool) {
        picture_sys_t *picsys = sys->picsys;
        IDirect3DSurface9_Release(picsys->surface);
        if (picsys->fallback)
            picture_Release(picsys->fallback);
        picture_pool_Delete(sys->pool);
    }
    sys->pool = NULL;
}
