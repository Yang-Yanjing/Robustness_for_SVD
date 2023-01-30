/* */
static picture_pool_t *Pool(vout_display_t *vd, unsigned count)
{
    vout_display_sys_t *sys = vd->sys;
    if (!sys->pool)
        sys->pool = vout_display_opengl_GetPool(sys->vgl, count);
    return sys->pool;
}
