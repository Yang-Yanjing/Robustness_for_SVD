}
static int DrawMeta(intf_thread_t *intf)
{
    intf_sys_t *sys = intf->p_sys;
    input_thread_t *p_input = sys->p_input;
    input_item_t *item;
    int l = 0;
    if (!p_input)
        return 0;
    item = input_GetItem(p_input);
    vlc_mutex_lock(&item->lock);
    for (int i=0; i<VLC_META_TYPE_COUNT; i++) {
        const char *meta = vlc_meta_Get(item->p_meta, i);
        if (!meta || !*meta)
            continue;
        if (sys->color) color_set(C_CATEGORY, NULL);
        MainBoxWrite(sys, l++, "  [%s]", vlc_meta_TypeToLocalizedString(i));
        if (sys->color) color_set(C_DEFAULT, NULL);
        MainBoxWrite(sys, l++, "      %s", meta);
    }
    vlc_mutex_unlock(&item->lock);
    return l;
}
