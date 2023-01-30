 */
static picture_pool_t *Pool (vout_display_t *vd, unsigned requested_count)
{
    vout_display_sys_t *sys = vd->sys;
    if (!sys->pool)
    {
        vlc_gl_MakeCurrent (sys->gl);
        sys->pool = vout_display_opengl_GetPool (sys->vgl, requested_count);
        vlc_gl_ReleaseCurrent (sys->gl);
    }
    return sys->pool;
}
