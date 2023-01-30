}
static void Close(vlc_object_t *object)
{
    filter_t     *filter = (filter_t *)object;
    filter_sys_t *sys    = filter->p_sys;
    var_DelCallback(filter, CFG_PREFIX "variance", Callback, NULL);
    vlc_mutex_destroy(&sys->cfg.lock);
    free(sys);
}
