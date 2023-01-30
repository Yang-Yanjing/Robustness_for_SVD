}
static void CycleESTrack(intf_sys_t *sys, const char *var)
{
    input_thread_t *input = sys->p_input;
    if (!input)
        return;
    vlc_value_t val;
    if (var_Change(input, var, VLC_VAR_GETLIST, &val, NULL) < 0)
        return;
    vlc_list_t *list = val.p_list;
    int64_t current = var_GetInteger(input, var);
    int i;
    for (i = 0; i < list->i_count; i++)
        if (list->p_values[i].i_int == current)
            break;
    if (++i >= list->i_count)
        i = 0;
    var_SetInteger(input, var, list->p_values[i].i_int);
}
