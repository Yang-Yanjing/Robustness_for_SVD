}
static int SubDrawObject(intf_sys_t *sys, int l, vlc_object_t *p_obj, int i_level, const char *prefix)
{
    char *name = vlc_object_get_name(p_obj);
    MainBoxWrite(sys, l++, "%*s%s%s \"%s\" (%p)", 2 * i_level++, "", prefix,
                  p_obj->psz_object_type, name ? name : "", p_obj);
    free(name);
    vlc_list_t *list = vlc_list_children(p_obj);
    for (int i = 0; i < list->i_count ; i++) {
        l = SubDrawObject(sys, l, list->p_values[i].p_object, i_level,
            (i == list->i_count - 1) ? "`-" : "|-" );
    }
    vlc_list_release(list);
    return l;
}
