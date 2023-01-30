 *****************************************************************************/
static int BarGraphCallback(vlc_object_t *p_this, char const *psz_var,
                         vlc_value_t oldval, vlc_value_t newval, void *p_data)
{
    VLC_UNUSED(p_this); VLC_UNUSED(oldval);
    filter_sys_t *p_sys = p_data;
    BarGraph_t *p_BarGraph = &p_sys->p_BarGraph;
    char* res = NULL;
    vlc_mutex_lock(&p_sys->lock);
    if (!strcmp(psz_var, CFG_PREFIX "x"))
        p_sys->i_pos_x = newval.i_int;
    else if (!strcmp(psz_var, CFG_PREFIX "y"))
        p_sys->i_pos_y = newval.i_int;
    else if (!strcmp(psz_var, CFG_PREFIX "position"))
        p_sys->i_pos = newval.i_int;
    else if (!strcmp(psz_var, CFG_PREFIX "transparency"))
        p_BarGraph->i_alpha = VLC_CLIP(newval.i_int, 0, 255);
    else if (!strcmp(psz_var, CFG_PREFIX "i_values")) {
        char *psz = xstrdup(newval.psz_string ? newval.psz_string : "");
        // in case many answer are received at the same time, only keep one
        res = strchr(psz, '@');
        if (res)
            *res = '\0';
        parse_i_values(p_BarGraph, psz);
        free(psz);
        Draw(p_BarGraph);
    } else if (!strcmp(psz_var, CFG_PREFIX "alarm")) {
        p_BarGraph->alarm = newval.b_bool;
        Draw(p_BarGraph);
    } else if (!strcmp(psz_var, CFG_PREFIX "barWidth")) {
        p_BarGraph->barWidth = newval.i_int;
        Draw(p_BarGraph);
    }
    p_sys->b_spu_update = true;
    vlc_mutex_unlock(&p_sys->lock);
    return VLC_SUCCESS;
}
