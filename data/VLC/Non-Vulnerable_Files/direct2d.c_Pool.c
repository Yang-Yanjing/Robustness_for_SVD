 */
static picture_pool_t *Pool(vout_display_t *vd, unsigned count)
{
    vout_display_sys_t *sys = vd->sys;
    if (!sys->pool) {
        sys->pool = picture_pool_NewFromFormat(&vd->fmt, count);
#ifndef NDEBUG
        msg_Dbg(vd, "New picture pool created");
#endif
    }
    return sys->pool;
}
