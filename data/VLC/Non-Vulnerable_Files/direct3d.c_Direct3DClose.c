 */
static void Direct3DClose(vout_display_t *vd)
{
    vout_display_sys_t *sys = vd->sys;
    Direct3DDestroyResources(vd);
    if (sys->d3ddev)
       IDirect3DDevice9_Release(sys->d3ddev);
    sys->d3ddev = NULL;
}
