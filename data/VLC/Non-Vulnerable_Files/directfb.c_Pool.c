/* */
static picture_pool_t *Pool(vout_display_t *vd, unsigned count)
{
    VLC_UNUSED(count);
    vout_display_sys_t *sys = vd->sys;
    IDirectFBSurface *primary = sys->primary;
    if (!sys->pool) {
        picture_resource_t rsc;
        memset(&rsc, 0, sizeof(rsc));
        rsc.p[0].i_lines  = vd->fmt.i_height;
        for (int i = 0; i < 3; i++) {
            rsc.p_sys = malloc(sizeof(*rsc.p_sys));
            if (!rsc.p_sys)
                goto cleanup;
            rsc.p_sys->sys = sys;
            void *pixels;
            int  pitch;
            if (primary->Lock(primary, DSLF_WRITE, &pixels, &pitch) != DFB_OK)
                goto cleanup;
            rsc.p[0].i_pitch = pitch;
            rsc.p[0].p_pixels = pixels;
            primary->Unlock(primary);
            primary->Flip(primary, NULL, 0);
            sys->pics[i] = picture_NewFromResource(&vd->fmt, &rsc);
            if (!sys->pics[i]) {
                free(rsc.p_sys);
                goto cleanup;
            }
        }
        picture_pool_configuration_t cfg = {
            .picture_count  = 3,
            .picture        = sys->pics,
            .lock           = Lock,
            .unlock         = NULL,
        };
        sys->pool = picture_pool_NewExtended(&cfg);
    }
    return sys->pool;
cleanup:
    for (int i = 0; i < 2; i++) {
        if (sys->pics[i]) {
            free(sys->pics[i]->p_sys);
            picture_Release(sys->pics[i]);
        }
    }
    return NULL;
}
