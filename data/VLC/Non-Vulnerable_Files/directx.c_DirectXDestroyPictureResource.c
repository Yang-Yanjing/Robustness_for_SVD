}
static void DirectXDestroyPictureResource(vout_display_t *vd)
{
    vout_display_sys_t *sys = vd->sys;
    if (sys->picsys->front_surface != sys->picsys->surface)
        DirectXDestroySurface(sys->picsys->surface);
    DirectXDestroySurface(sys->picsys->front_surface);
    if (sys->picsys->fallback)
        picture_Release(sys->picsys->fallback);
}
