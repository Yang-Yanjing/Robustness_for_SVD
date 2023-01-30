 *****************************************************************************/
static void PlaylistDestroy(intf_sys_t *sys)
{
    while (sys->plist_entries) {
        struct pl_item_t *p_pl_item = sys->plist[--sys->plist_entries];
        free(p_pl_item->display);
        free(p_pl_item);
    }
    free(sys->plist);
    sys->plist = NULL;
}
