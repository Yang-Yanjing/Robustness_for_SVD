 */
static void Direct3DDestroy(vout_display_t *vd)
{
    vout_display_sys_t *sys = vd->sys;
    if (sys->d3dobj)
       IDirect3D9_Release(sys->d3dobj);
    if (sys->hd3d9_dll)
        FreeLibrary(sys->hd3d9_dll);
    if (sys->hd3d9x_dll)
        FreeLibrary(sys->hd3d9x_dll);
    sys->d3dobj = NULL;
    sys->hd3d9_dll = NULL;
    sys->hd3d9x_dll = NULL;
}
