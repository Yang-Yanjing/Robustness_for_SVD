}
static int DrawInfo(intf_thread_t *intf)
{
    intf_sys_t *sys = intf->p_sys;
    input_thread_t *p_input = sys->p_input;
    input_item_t *item;
    int l = 0;
    if (!p_input)
        return 0;
    item = input_GetItem(p_input);
    vlc_mutex_lock(&item->lock);
    for (int i = 0; i < item->i_categories; i++) {
        info_category_t *p_category = item->pp_categories[i];
        if (sys->color) color_set(C_CATEGORY, NULL);
        MainBoxWrite(sys, l++, _("  [%s]"), p_category->psz_name);
        if (sys->color) color_set(C_DEFAULT, NULL);
        for (int j = 0; j < p_category->i_infos; j++) {
            info_t *p_info = p_category->pp_infos[j];
            MainBoxWrite(sys, l++, _("      %s: %s"),
                         p_info->psz_name, p_info->psz_value);
        }
    }
    vlc_mutex_unlock(&item->lock);
    return l;
}
