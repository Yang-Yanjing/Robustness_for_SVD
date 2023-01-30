}
static bool PlaylistAddChild(intf_sys_t *sys, playlist_item_t *p_child,
                             const char *c, const char d)
{
    int ret;
    char *name = input_item_GetTitleFbName(p_child->p_input);
    struct pl_item_t *p_pl_item = malloc(sizeof *p_pl_item);
    if (!name || !p_pl_item)
        goto error;
    p_pl_item->item = p_child;
    if (c && *c)
        ret = asprintf(&p_pl_item->display, "%s%c-%s", c, d, name);
    else
        ret = asprintf(&p_pl_item->display, " %s", name);
    free(name);
    name = NULL;
    if (ret == -1)
        goto error;
    INSERT_ELEM(sys->plist, sys->plist_entries,
                 sys->plist_entries, p_pl_item);
    return true;
error:
    free(name);
    free(p_pl_item);
    return false;
}
