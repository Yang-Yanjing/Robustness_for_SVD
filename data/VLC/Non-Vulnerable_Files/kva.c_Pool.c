 */
static picture_pool_t *Pool(vout_display_t *vd, unsigned count)
{
    vout_display_sys_t *sys = vd->sys;
    VLC_UNUSED(count);
    return sys->pool;
}
