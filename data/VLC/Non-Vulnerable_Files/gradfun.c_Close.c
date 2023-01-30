}
static void Close(vlc_object_t *object)
{
    filter_t     *filter = (filter_t *)object;
    filter_sys_t *sys = filter->p_sys;
    var_DelCallback(filter, CFG_PREFIX "radius",   Callback, NULL);
    var_DelCallback(filter, CFG_PREFIX "strength", Callback, NULL);
    vlc_free(sys->cfg.buf);
    vlc_mutex_destroy(&sys->lock);
    free(sys);
}
