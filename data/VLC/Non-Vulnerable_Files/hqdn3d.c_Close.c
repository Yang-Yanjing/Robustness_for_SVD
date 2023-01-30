 *****************************************************************************/
static void Close(vlc_object_t *this)
{
    filter_t *filter = (filter_t *)this;
    filter_sys_t *sys = filter->p_sys;
    struct vf_priv_s *cfg = &sys->cfg;
    var_DelCallback( filter, FILTER_PREFIX "luma-spat", DenoiseCallback, sys );
    var_DelCallback( filter, FILTER_PREFIX "chroma-spat", DenoiseCallback, sys );
    var_DelCallback( filter, FILTER_PREFIX "luma-temp", DenoiseCallback, sys );
    var_DelCallback( filter, FILTER_PREFIX "chroma-temp", DenoiseCallback, sys );
    vlc_mutex_destroy( &sys->coefs_mutex );
    for (int i = 0; i < 3; ++i) {
        free(cfg->Frame[i]);
    }
    free(cfg->Line);
    free(sys);
}
