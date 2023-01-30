 */
static void Close(vlc_object_t *p_this)
{
    filter_t *p_filter = (filter_t *)p_this;
    filter_sys_t *p_sys = p_filter->p_sys;
    for (int i = 0; ppsz_filter_callbacks[i]; i++)
        var_DelCallback(p_filter, ppsz_filter_callbacks[i],
                         BarGraphCallback, p_sys);
    var_DelCallback(p_filter->p_libvlc, CFG_PREFIX "i_values",
                    BarGraphCallback, p_sys);
    var_DelCallback(p_filter->p_libvlc, CFG_PREFIX "alarm",
                    BarGraphCallback, p_sys);
    var_Destroy(p_filter->p_libvlc, CFG_PREFIX "i_values");
    var_Destroy(p_filter->p_libvlc, CFG_PREFIX "alarm");
    if (p_sys->p_blend)
        filter_DeleteBlend(p_sys->p_blend);
    vlc_mutex_destroy(&p_sys->lock);
    if (p_sys->p_BarGraph.p_pic)
        picture_Release(p_sys->p_BarGraph.p_pic);
    free(p_sys->p_BarGraph.i_values);
    free(p_sys);
}
