/* */
static picture_pool_t *Pool(vout_display_t *vd, unsigned count)
{
    vout_display_sys_t *sys = vd->sys;
    if (!sys->pool) {
        if (!sys->picture) {
            picture_resource_t rsc;
            memset(&rsc, 0, sizeof(rsc));
            rsc.p[0].p_pixels = sys->video_ptr;
            rsc.p[0].i_lines  = sys->var_info.yres;
            rsc.p[0].i_pitch = sys->line_length;
            sys->picture = picture_NewFromResource(&vd->fmt, &rsc);
            if (!sys->picture)
                return NULL;
        }
        if (sys->is_hw_accel)
            sys->pool = picture_pool_New(1, &sys->picture);
        else
            sys->pool = picture_pool_NewFromFormat(&vd->fmt, count);
    }
    return sys->pool;
}
