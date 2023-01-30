/* Playlist suxx */
static int SubSearchPlaylist(intf_sys_t *sys, char *searchstring,
                              int i_start, int i_stop)
{
    for (int i = i_start + 1; i < i_stop; i++)
        if (strcasestr(sys->plist[i]->display, searchstring))
            return i;
    return -1;
}
