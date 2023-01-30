}
static void DirectXDestroyPool(vout_display_t *vd)
{
    vout_display_sys_t *sys = vd->sys;
    if (sys->pool) {
        DirectXDestroyPictureResource(vd);
        picture_pool_Delete(sys->pool);
    }
    sys->pool = NULL;
}
