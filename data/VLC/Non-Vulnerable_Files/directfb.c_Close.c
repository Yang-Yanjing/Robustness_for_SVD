}
static void Close(vlc_object_t *object)
{
    vout_display_t *vd = (vout_display_t *)object;
    vout_display_sys_t *sys = vd->sys;
    if (sys->pool)
        picture_pool_Delete(sys->pool);
    IDirectFBSurface *primary = sys->primary;
    if (primary)
        primary->Release(primary);
    IDirectFB *directfb = sys->directfb;
    if (directfb)
        directfb->Release(directfb);
    free(sys);
}
