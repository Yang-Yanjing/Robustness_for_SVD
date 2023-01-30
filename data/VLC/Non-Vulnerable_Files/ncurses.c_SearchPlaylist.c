}
static void SearchPlaylist(intf_sys_t *sys)
{
    char *str = sys->search_chain;
    int i_first = sys->box_idx;
    if (i_first < 0)
        i_first = 0;
    if (!str || !*str)
        return;
    int i_item = SubSearchPlaylist(sys, str, i_first + 1, sys->plist_entries);
    if (i_item < 0)
        i_item = SubSearchPlaylist(sys, str, 0, i_first);
    if (i_item > 0) {
        sys->box_idx = i_item;
        CheckIdx(sys);
    }
}
