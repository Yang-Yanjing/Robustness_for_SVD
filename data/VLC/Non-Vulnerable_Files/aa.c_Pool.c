 */
static picture_pool_t *Pool(vout_display_t *vd, unsigned count)
{
    vout_display_sys_t *sys = vd->sys;
    VLC_UNUSED(count);
    if (!sys->pool) {
        picture_resource_t rsc;
        memset(&rsc, 0, sizeof(rsc));
        rsc.p[0].p_pixels = aa_image(sys->aa_context);
        rsc.p[0].i_pitch = aa_imgwidth(sys->aa_context);
        rsc.p[0].i_lines = aa_imgheight(sys->aa_context);
        picture_t *p_picture = picture_NewFromResource(&vd->fmt, &rsc);
        if (!p_picture)
            return NULL;
        sys->pool = picture_pool_New(1, &p_picture);
    }
    return sys->pool;
}
